/*
 * Copyright (c) 2019 - 2020 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <geode/mesh/core/triangulated_surface_view.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/detail/surface_mesh_view_impl.h>

namespace geode
{
    template < index_t dimension >
    class TriangulatedSurfaceView< dimension >::Impl
        : public detail::SurfaceMeshViewImpl< dimension >
    {
    public:
        Impl( TriangulatedSurfaceView< dimension >& surface_view,
            const TriangulatedSurface< dimension >& surface )
            : detail::SurfaceMeshViewImpl< dimension >( surface_view, surface )
        {
        }
    };

    template < index_t dimension >
    TriangulatedSurfaceView< dimension >::TriangulatedSurfaceView(
        const TriangulatedSurface< dimension >& surface )
        : impl_( *this, surface )
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceView< dimension >::TriangulatedSurfaceView(
        TriangulatedSurfaceView&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceView< dimension >::~TriangulatedSurfaceView() // NOLINT
    {
    }

    template < index_t dimension >
    index_t TriangulatedSurfaceView< dimension >::viewed_vertex(
        index_t vertex_id ) const
    {
        return impl_->viewed_vertex( vertex_id );
    }

    template < index_t dimension >
    const Point< dimension >& TriangulatedSurfaceView< dimension >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    template < index_t dimension >
    index_t TriangulatedSurfaceView< dimension >::get_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        return impl_->get_polygon_vertex( polygon_vertex );
    }

    template < index_t dimension >
    absl::optional< index_t >
        TriangulatedSurfaceView< dimension >::get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const
    {
        return impl_->get_polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    index_t TriangulatedSurfaceView< dimension >::add_viewed_vertex(
        index_t vertex_id, TriangulatedSurfaceViewKey )
    {
        return impl_->add_viewed_vertex( vertex_id );
    }

    template < index_t dimension >
    index_t TriangulatedSurfaceView< dimension >::viewed_triangle(
        index_t triangle_id ) const
    {
        return impl_->viewed_polygon( triangle_id );
    }

    template < index_t dimension >
    index_t TriangulatedSurfaceView< dimension >::add_viewed_triangle(
        index_t triangle_id, TriangulatedSurfaceViewKey )
    {
        return impl_->add_viewed_polygon( triangle_id );
    }

    template < index_t dimension >
    absl::optional< PolygonVertex >
        TriangulatedSurfaceView< dimension >::get_polygon_around_vertex(
            index_t vertex_id ) const
    {
        return impl_->get_polygon_around_vertex( vertex_id );
    }

    template class opengeode_mesh_api TriangulatedSurfaceView< 2 >;
    template class opengeode_mesh_api TriangulatedSurfaceView< 3 >;
} // namespace geode
