/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include "../../../common.hpp"

#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/block_collections.hpp>

#define PYTHON_BLOCK_COLLECTIONS( dimension )                                  \
    const auto name##dimension =                                               \
        "BlockCollections" + std::to_string( dimension ) + "D";                \
    pybind11::class_< BlockCollections##dimension##D >(                        \
        module, name##dimension.c_str() )                                      \
        .def( "nb_block_collections",                                          \
            &BlockCollections##dimension##D::nb_block_collections )            \
        .def( "block_collection",                                              \
            &BlockCollections##dimension##D::block_collection,                 \
            pybind11::return_value_policy::reference )                         \
        .def(                                                                  \
            "block_collections",                                               \
            []( const BlockCollections##dimension##D& self ) {                 \
                std::vector< const BlockCollection##dimension##D* >            \
                    components;                                                \
                for( const auto& component : self.block_collections() )        \
                {                                                              \
                    components.push_back( &component );                        \
                }                                                              \
                return components;                                             \
            },                                                                 \
            pybind11::return_value_policy::reference )

namespace geode
{
    void define_block_collections( pybind11::module& module )
    {
        PYTHON_BLOCK_COLLECTIONS( 2 );
        PYTHON_BLOCK_COLLECTIONS( 3 );
    }
} // namespace geode
