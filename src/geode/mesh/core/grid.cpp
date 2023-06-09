/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/mesh/core/grid.h>

#include <absl/container/inlined_vector.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

namespace
{
    template < geode::index_t dimension >
    bool vertex_is_on_axis_origin(
        geode::local_index_t vertex_id, geode::local_index_t axis_id )
    {
        /* returns ((vertex_id / pow(2,axis_id)) modulo 2) using binary
         * operators (faster). */
        return ( ( vertex_id / ( 1 << axis_id ) ) & 1 ) == 0;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class Grid< dimension >::Impl
    {
    public:
        Impl() = default;

        double cell_length_in_direction( index_t direction ) const
        {
            return cells_length_[direction];
        }

        double cell_size() const
        {
            double cell_size{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                cell_size *= cell_length_in_direction( d );
            }
            return cell_size;
        }

        index_t nb_vertices( const Grid< dimension >& grid ) const
        {
            index_t result{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                result *= nb_vertices_in_direction( grid, d );
            }
            return result;
        }

        Point< dimension > point(
            const Grid< dimension >& grid, const VertexIndices& index ) const
        {
            auto point = grid.origin();
            for( const auto d : LRange{ dimension } )
            {
                point.set_value(
                    d, point.value( d )
                           + index[d] * cell_length_in_direction( d ) );
            }
            return point;
        }

        BoundingBox< dimension > grid_bounding_box(
            const Grid< dimension >& grid ) const
        {
            BoundingBox< dimension > bbox;
            bbox.add_point( grid.origin() );
            VertexIndices last;
            for( const auto d : LRange{ dimension } )
            {
                last[d] = nb_vertices_in_direction( grid, d ) - 1;
            }
            bbox.add_point( point( grid, last ) );
            return bbox;
        }

        index_t nb_vertices_in_direction(
            const Grid< dimension >& grid, index_t direction ) const
        {
            return grid.nb_cells_in_direction( direction ) + 1;
        }

        bool is_vertex_on_border(
            const Grid< dimension >& grid, const VertexIndices& index ) const
        {
            for( const auto d : LRange{ dimension } )
            {
                if( index[d] == 0
                    || index[d] == nb_vertices_in_direction( grid, d ) - 1 )
                {
                    return true;
                }
            }
            return false;
        }

        index_t nb_vertices_on_borders( const Grid< dimension >& grid ) const
        {
            index_t nb_inside_vertices{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                const auto nb_vertices_d = nb_vertices_in_direction( grid, d );
                if( nb_vertices_d < 2 )
                {
                    nb_inside_vertices *= nb_vertices_d;
                }
                else
                {
                    nb_inside_vertices *= nb_vertices_d - 2;
                }
            }
            return nb_vertices( grid ) - nb_inside_vertices;
        }

        CellVertices cell_vertices( const CellIndices& cell_id ) const
        {
            CellVertices cell_vertices;
            for( const auto vertex_local_id :
                LRange{ nb_cell_vertices_static() } )
            {
                cell_vertices[vertex_local_id] =
                    cell_vertex_indices( cell_id, vertex_local_id );
            }
            return cell_vertices;
        }

        VertexIndices cell_vertex_indices(
            const CellIndices& cell_id, local_index_t vertex_id ) const
        {
            auto node_index = cell_id;
            for( const auto d : LRange{ dimension } )
            {
                if( !vertex_is_on_axis_origin< dimension >( vertex_id, d ) )
                {
                    node_index[d] += 1;
                }
            }
            return node_index;
        }

        absl::optional< VertexIndices > next_vertex(
            const Grid< dimension >& grid,
            const VertexIndices& index,
            index_t direction ) const
        {
            if( index[direction] + 1
                < nb_vertices_in_direction( grid, direction ) )
            {
                auto next = index;
                next[direction]++;
                return next;
            }
            return absl::nullopt;
        }

        absl::optional< VertexIndices > previous_vertex(
            const VertexIndices& index, index_t direction ) const
        {
            if( index[direction] > 0 )
            {
                auto prev = index;
                prev[direction]--;
                return prev;
            }
            return absl::nullopt;
        }

        bool contains( const Grid< dimension >& grid,
            const geode::Point< dimension >& origin,
            const Point< dimension >& query ) const
        {
            for( const auto d : LRange{ dimension } )
            {
                const auto value =
                    point_local_grid_coordinate( origin, query, d );
                if( value < -global_epsilon
                    || value
                           > grid.nb_cells_in_direction( d ) + global_epsilon )
                {
                    return false;
                }
            }
            return true;
        }

        VertexIndices closest_vertex( const Grid< dimension >& grid,
            const geode::Point< dimension >& origin,
            const Point< dimension >& query ) const
        {
            VertexIndices closest;
            for( const auto d : LRange{ dimension } )
            {
                const auto value =
                    point_local_grid_coordinate( origin, query, d );
                if( value < 0. )
                {
                    closest[d] = 0;
                }
                else if( value > grid.nb_cells_in_direction( d ) )
                {
                    closest[d] = grid.nb_cells_in_direction( d );
                }
                else
                {
                    closest[d] = static_cast< index_t >( std::round( value ) );
                }
            }
            return closest;
        }

        Point< dimension > cell_barycenter(
            const Grid< dimension >& grid, const CellIndices& cell_id ) const
        {
            auto other = cell_id;
            for( const auto d : LRange{ dimension } )
            {
                other[d]++;
            }
            return ( point( grid, cell_id ) + point( grid, other ) ) / 2.;
        }

        CellsAroundVertex cells( const Grid< dimension >& grid,
            const geode::Point< dimension >& origin,
            const Point< dimension >& query ) const
        {
            CellIndices min;
            CellIndices max;
            for( const auto d : LRange{ dimension } )
            {
                const auto value =
                    point_local_grid_coordinate( origin, query, d );
                if( value < -global_epsilon
                    || value
                           > grid.nb_cells_in_direction( d ) + global_epsilon )
                {
                    return {};
                }
                auto floating_floor = std::floor( value );
                if( floating_floor < 0 )
                {
                    floating_floor = 0;
                }
                else if( floating_floor > grid.nb_cells_in_direction( d ) - 1 )
                {
                    floating_floor = grid.nb_cells_in_direction( d ) - 1;
                }
                const auto integer_floor =
                    static_cast< index_t >( floating_floor );
                min[d] = integer_floor;
                max[d] = integer_floor;
                const auto remainder = value - floating_floor;
                if( remainder < global_epsilon )
                {
                    min[d] = integer_floor > 0 ? integer_floor - 1 : 0;
                }
                else if( remainder > 1 - global_epsilon )
                {
                    max[d] = std::min( integer_floor + 1,
                        grid.nb_cells_in_direction( d ) - 1 );
                }
            }
            CellsAroundVertex cells_around_point;
            cells_around_point.push_back( min );
            for( const auto d : LRange{ dimension } )
            {
                if( max[d] != min[d] )
                {
                    for( const auto& cell_indices : cells_around_point )
                    {
                        cells_around_point.push_back( cell_indices );
                        cells_around_point.back()[d] = max[d];
                    }
                }
            }
            return cells_around_point;
        }

        const std::array< double, dimension >& cells_lengths() const
        {
            return cells_length_;
        }

        void set_grid_dimensions( Grid< dimension >& grid,
            std::array< double, dimension > cells_length )
        {
            cells_length_ = std::move( cells_length );
            for( const auto direction : LRange{ dimension } )
            {
                OPENGEODE_EXCEPTION( cells_length_[direction] > global_epsilon,
                    "[Grid] Creation of a grid with a cell length "
                    "smaller than epsilon in direction ",
                    direction, "." );
            }
            double nb_vertices_double{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                nb_vertices_double *= static_cast< double >(
                    grid.nb_cells_in_direction( d ) + 1 );
            }
            OPENGEODE_EXCEPTION(
                nb_vertices_double < static_cast< double >( UINT_MAX ),
                "[Grid] Creation of a grid for which the number of cell "
                "vertices exceeds the unsigned int limit." );
        }

        void copy( const Grid< dimension >::Impl& impl )
        {
            cells_length_ = impl.cells_length_;
        }

        std::array< index_t, dimension >&& deprecated_cells_number()
        {
            return std::move( deprecated_cells_number_ );
        }

    private:
        double point_local_grid_coordinate(
            const geode::Point< dimension >& origin,
            const geode::Point< dimension >& query,
            local_index_t axis ) const
        {
            return ( query.value( axis ) - origin.value( axis ) )
                   / cells_length_[axis];
        }

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{
                                    { []( Archive& a, Impl& impl ) {
                                         a.container4b(
                                             impl.deprecated_cells_number_ );
                                         a.container8b( impl.cells_length_ );
                                     },
                                        []( Archive& a, Impl& impl ) {
                                            a.container8b( impl.cells_length_ );
                                        } } } );
        }

    private:
        std::array< index_t, dimension > deprecated_cells_number_;
        std::array< double, dimension > cells_length_;
    };

    template < index_t dimension >
    Grid< dimension >::Grid()
    {
    }

    template < index_t dimension >
    Grid< dimension >::Grid( Grid&& other ) : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Grid< dimension >& Grid< dimension >::operator=( Grid&& other )
    {
        impl_ = std::move( other.impl_ );
        return *this;
    }

    template < index_t dimension >
    Grid< dimension >::~Grid() // NOLINT
    {
    }

    template < index_t dimension >
    double Grid< dimension >::cell_length_in_direction(
        index_t direction ) const
    {
        return impl_->cell_length_in_direction( direction );
    }

    template < index_t dimension >
    double Grid< dimension >::cell_size() const
    {
        return impl_->cell_size();
    }

    template < index_t dimension >
    Point< dimension > Grid< dimension >::grid_point(
        const VertexIndices& index ) const
    {
        return impl_->point( *this, index );
    }

    template < index_t dimension >
    index_t Grid< dimension >::nb_grid_vertices() const
    {
        return impl_->nb_vertices( *this );
    }

    template < index_t dimension >
    index_t Grid< dimension >::nb_vertices_in_direction(
        index_t direction ) const
    {
        return impl_->nb_vertices_in_direction( *this, direction );
    }

    template < index_t dimension >
    index_t Grid< dimension >::nb_vertices_on_borders() const
    {
        return impl_->nb_vertices_on_borders( *this );
    }

    template < index_t dimension >
    bool Grid< dimension >::is_grid_vertex_on_border(
        const VertexIndices& index ) const
    {
        return impl_->is_vertex_on_border( *this, index );
    }

    template < index_t dimension >
    auto Grid< dimension >::cell_vertices( const CellIndices& cell_id ) const
        -> CellVertices
    {
        return impl_->cell_vertices( cell_id );
    }

    template < index_t dimension >
    auto Grid< dimension >::cell_vertex_indices( const CellIndices& cell_id,
        local_index_t vertex_id ) const -> VertexIndices
    {
        return impl_->cell_vertex_indices( cell_id, vertex_id );
    }

    template < index_t dimension >
    auto Grid< dimension >::next_vertex( const CellIndices& index,
        index_t direction ) const -> absl::optional< CellIndices >
    {
        return impl_->next_vertex( *this, index, direction );
    }

    template < index_t dimension >
    auto Grid< dimension >::previous_vertex( const CellIndices& index,
        index_t direction ) const -> absl::optional< CellIndices >
    {
        return impl_->previous_vertex( index, direction );
    }

    template < index_t dimension >
    bool Grid< dimension >::contains( const Point< dimension >& query ) const
    {
        return impl_->contains( *this, origin(), query );
    }

    template < index_t dimension >
    auto Grid< dimension >::closest_vertex(
        const Point< dimension >& query ) const -> VertexIndices
    {
        return impl_->closest_vertex( *this, origin(), query );
    }

    template < index_t dimension >
    auto Grid< dimension >::cells( const Point< dimension >& query ) const
        -> CellsAroundVertex
    {
        return impl_->cells( *this, origin(), query );
    }

    template < index_t dimension >
    Point< dimension > Grid< dimension >::cell_barycenter(
        const CellIndices& cell_id ) const
    {
        return impl_->cell_barycenter( *this, cell_id );
    }

    template < index_t dimension >
    BoundingBox< dimension > Grid< dimension >::grid_bounding_box() const
    {
        return impl_->grid_bounding_box( *this );
    }

    template < index_t dimension >
    void Grid< dimension >::set_grid_dimensions(
        std::array< index_t, dimension > cells_number,
        std::array< double, dimension > cells_length,
        GridKey )
    {
        set_array_dimensions( std::move( cells_number ) );
        impl_->set_grid_dimensions( *this, std::move( cells_length ) );
    }

    template < index_t dimension >
    void Grid< dimension >::copy( const Grid< dimension >& grid, GridKey )
    {
        CellArray< dimension >::copy( grid );
        impl_->copy( *grid.impl_ );
    }

    template < index_t dimension >
    template < typename Archive >
    void Grid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive,
                Grid >{ { []( Archive& a, Grid& grid ) {
                             a.object( grid.impl_ );
                             grid.set_array_dimensions(
                                 grid.impl_->deprecated_cells_number() );
                         },
                []( Archive& a, Grid& grid ) {
                    a.ext( grid,
                        bitsery::ext::BaseClass< CellArray< dimension > >{} );
                    a.object( grid.impl_ );
                } } } );
    }

    template class opengeode_mesh_api Grid< 2 >;
    template class opengeode_mesh_api Grid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, Grid< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, Grid< 3 > );
} // namespace geode
