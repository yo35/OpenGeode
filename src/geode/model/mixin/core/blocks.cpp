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

#include <geode/model/mixin/core/blocks.h>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/io/polyhedral_solid_input.h>
#include <geode/mesh/io/polyhedral_solid_output.h>
#include <geode/mesh/io/tetrahedral_solid_input.h>
#include <geode/mesh/io/tetrahedral_solid_output.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/detail/components_storage.h>

namespace geode
{
    template < index_t dimension >
    class Blocks< dimension >::Impl
        : public detail::ComponentsStorage< Block< dimension > >
    {
    };

    template < index_t dimension >
    Blocks< dimension >::Blocks() // NOLINT
    {
    }

    template < index_t dimension >
    Blocks< dimension >::Blocks( Blocks&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Blocks< dimension >::~Blocks() // NOLINT
    {
    }

    template < index_t dimension >
    index_t Blocks< dimension >::nb_blocks() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const Block< dimension >& Blocks< dimension >::block( const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Block< dimension >& Blocks< dimension >::modifiable_block( const uuid& id )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void Blocks< dimension >::save_blocks( absl::string_view directory ) const
    {
        const auto prefix = absl::StrCat(
            directory, "/", Block< dimension >::component_type_static().get() );
        for( const auto& block : blocks() )
        {
            const auto& mesh = block.mesh();
            const auto file = absl::StrCat(
                prefix, block.id().string(), ".", mesh.native_extension() );

            if( const auto* tetra =
                    dynamic_cast< const TetrahedralSolid< dimension >* >(
                        &mesh ) )
            {
                save_tetrahedral_solid( *tetra, file );
            }
            else if( const auto* poly =
                         dynamic_cast< const PolyhedralSolid< dimension >* >(
                             &mesh ) )
            {
                save_polyhedral_solid( *poly, file );
            }
            else
            {
                throw OpenGeodeException(
                    "[Blocks::save_blocks] Cannot find the explicit "
                    "SolidMesh type" );
            }
        }
        impl_->save_components( absl::StrCat( directory, "/blocks" ) );
    }

    template < index_t dimension >
    void Blocks< dimension >::load_blocks( absl::string_view directory )
    {
        impl_->load_components( absl::StrCat( directory, "/blocks" ) );
        for( auto& block : modifiable_blocks() )
        {
            const auto file =
                impl_->find_file( directory, block.component_id() );
            if( MeshFactory::type( block.mesh_type() )
                == TetrahedralSolid< dimension >::type_name_static() )
            {
                block.set_mesh( load_tetrahedral_solid< dimension >(
                                    block.mesh_type(), file ),
                    typename Block< dimension >::BlocksKey{} );
            }
            else
            {
                block.set_mesh( load_polyhedral_solid< dimension >(
                                    block.mesh_type(), file ),
                    typename Block< dimension >::BlocksKey{} );
            }
        }
    }

    template < index_t dimension >
    const uuid& Blocks< dimension >::create_block()
    {
        typename Blocks< dimension >::Impl::ComponentPtr block{
            new Block< dimension >{ typename Block< dimension >::BlocksKey() }
        };
        const auto& id = block->id();
        impl_->add_component( std::move( block ) );
        return id;
    }

    template < index_t dimension >
    const uuid& Blocks< dimension >::create_block( const MeshImpl& impl )
    {
        typename Blocks< dimension >::Impl::ComponentPtr block{
            new Block< dimension >{ impl, {} }
        };
        const auto& id = block->id();
        impl_->add_component( std::move( block ) );
        return id;
    }

    template < index_t dimension >
    void Blocks< dimension >::delete_block( const Block< dimension >& block )
    {
        impl_->delete_component( block.id() );
    }

    template < index_t dimension >
    typename Blocks< dimension >::BlockRange Blocks< dimension >::blocks() const
    {
        return { *this };
    }

    template < index_t dimension >
    class Blocks< dimension >::BlockRangeBase::Impl
        : public BaseRange< typename Blocks< dimension >::Impl::Iterator >
    {
        using Iterator = typename Blocks< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Iterator >( begin, end )
        {
        }

        Block< dimension >& block() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    Blocks< dimension >::BlockRangeBase::BlockRangeBase( const Blocks& blocks )
        : impl_( blocks.impl_->begin(), blocks.impl_->end() )
    {
    }

    template < index_t dimension >
    Blocks< dimension >::BlockRangeBase::BlockRangeBase(
        BlockRangeBase&& other ) noexcept
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Blocks< dimension >::BlockRangeBase::BlockRangeBase(
        const BlockRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Blocks< dimension >::BlockRangeBase::~BlockRangeBase() // NOLINT
    {
    }

    template < index_t dimension >
    bool Blocks< dimension >::BlockRangeBase::operator!=(
        const BlockRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Blocks< dimension >::BlockRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    Blocks< dimension >::BlockRange::BlockRange( const Blocks& blocks )
        : BlockRangeBase( blocks ), BeginEnd< BlockRange >( *this )
    {
    }

    template < index_t dimension >
    const Block< dimension >& Blocks< dimension >::BlockRange::operator*() const
    {
        return this->impl_->block();
    }

    template < index_t dimension >
    typename Blocks< dimension >::ModifiableBlockRange
        Blocks< dimension >::modifiable_blocks()
    {
        return { *this };
    }

    template < index_t dimension >
    Blocks< dimension >::ModifiableBlockRange::ModifiableBlockRange(
        const Blocks& blocks )
        : BlockRangeBase( blocks ), BeginEnd< ModifiableBlockRange >( *this )
    {
    }

    template < index_t dimension >
    Block< dimension >&
        Blocks< dimension >::ModifiableBlockRange::operator*() const
    {
        return this->impl_->block();
    }

    template class opengeode_model_api Blocks< 3 >;
} // namespace geode
