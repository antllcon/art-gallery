#pragma once

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Triangulation_conformer_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <list>
#include <vector>

class PolygonTriangulator
{
public:
	using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
	using Point = Kernel::Point_2;
	using Polygon_2 = CGAL::Polygon_2<Kernel>;
	using TriangleList = std::vector<std::tuple<Point, Point, Point>>;

private:
	struct FaceInfo
	{
		int in_domain = -1;
	};

	using Vb = CGAL::Triangulation_vertex_base_2<Kernel>;
	using Fbb = CGAL::Constrained_triangulation_face_base_2<Kernel>;
	using Fb = CGAL::Triangulation_face_base_with_info_2<FaceInfo, Kernel, Fbb>;
	using TDS = CGAL::Triangulation_data_structure_2<Vb, Fb>;
	using CDTBase = CGAL::Constrained_Delaunay_triangulation_2<Kernel, TDS, CGAL::Exact_predicates_tag>;
	using CDT = CGAL::Constrained_triangulation_plus_2<CDTBase>;

	Polygon_2 outer;
	std::vector<Polygon_2> holes;
	CDT cdt;

	void InsertPolygon(const Polygon_2& poly)
	{
		for (std::size_t i = 0; i < poly.size(); ++i)
			cdt.insert_constraint(poly[i], poly[(i + 1) % poly.size()]);
	}

	void mark_domains()
	{
		for (auto f : cdt.all_face_handles())
			f->info().in_domain = -1;

		std::list<typename CDT::Edge> border;
		mark_domains(cdt.infinite_face(), 0, border);

		while (!border.empty())
		{
			auto e = border.front();
			border.pop_front();
			auto neighbor = e.first->neighbor(e.second);
			if (neighbor->info().in_domain == -1)
				mark_domains(neighbor, 1, border);
		}
	}

	void mark_domains(typename CDT::Face_handle start, int index, std::list<typename CDT::Edge>& border)
	{
		if (start->info().in_domain != -1)
			return;
		std::list<typename CDT::Face_handle> queue{ start };
		while (!queue.empty())
		{
			auto fh = queue.front();
			queue.pop_front();
			if (fh->info().in_domain == -1)
			{
				fh->info().in_domain = index;
				for (int i = 0; i < 3; ++i)
				{
					auto n = fh->neighbor(i);
					if (n->info().in_domain == -1)
					{
						if (cdt.is_constrained({ fh, i }))
							border.push_back({ fh, i });
						else
							queue.push_back(n);
					}
				}
			}
		}
	}

public:
	void SetOuterPolygon(const Polygon_2& poly)
	{
		outer = poly;
		if (!outer.is_simple())
			throw std::runtime_error("Outer polygon is not simple");
		if (outer.orientation() != CGAL::COUNTERCLOCKWISE)
			outer.reverse_orientation();
	}

	void AddHole(const Polygon_2& hole)
	{
		if (!hole.is_simple())
			throw std::runtime_error("Hole is not simple");
		Polygon_2 h = hole;
		if (h.orientation() != CGAL::CLOCKWISE)
			h.reverse_orientation();
		holes.push_back(h);
	}

	// Добавить больше проверок
	void Triangulate()
	{
		if (outer.is_empty())
		{
			throw std::runtime_error("Нет внешней границы");
		}
		cdt.clear();
		InsertPolygon(outer);
		for (const auto& h : holes)
			InsertPolygon(h);
		mark_domains();
	}

	TriangleList GetTriangles() const
	{
		TriangleList result;

		for (auto f = cdt.finite_faces_begin(); f != cdt.finite_faces_end(); ++f)
		{
			if (f->info().in_domain != 1)
				continue;

			Point a = f->vertex(0)->point();
			Point b = f->vertex(1)->point();
			Point c = f->vertex(2)->point();
			Point center((a.x() + b.x() + c.x()) / 3, (a.y() + b.y() + c.y()) / 3);

			bool inside_hole = false;
			for (const auto& h : holes)
			{
				if (h.bounded_side(center) == CGAL::ON_BOUNDED_SIDE)
				{
					inside_hole = true;
					break;
				}
			}
			if (!inside_hole)
				result.emplace_back(a, b, c);
		}
		return result;
	}
};
