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

#include "../../basic/factory.h"
#include "../../basic/input.h"
#include "../../common.h"

#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/io/polyhedral_solid_input.h>
#include <geode/mesh/io/polyhedral_solid_output.h>

#define PYTHON_POLYHEDRAL_SOLID_IO( dimension )                                \
    const auto save##dimension =                                               \
        "save_polyhedral_solid" + std::to_string( dimension ) + "D";           \
    module.def(                                                                \
        save##dimension.c_str(), &save_polyhedral_solid< dimension > );        \
    const auto load##dimension =                                               \
        "load_polyhedral_solid" + std::to_string( dimension ) + "D";           \
    module.def( load##dimension.c_str(),                                       \
        static_cast< std::unique_ptr< PolyhedralSolid< dimension > > ( * )(    \
            absl::string_view ) >( &load_polyhedral_solid< dimension > ) );    \
    const auto check##dimension = "check_polyhedral_solid_missing_files"       \
                                  + std::to_string( dimension ) + "D";         \
    module.def( check##dimension.c_str(),                                      \
        &check_polyhedral_solid_missing_files< dimension > );                  \
    PYTHON_INPUT_CLASS( std::unique_ptr< PolyhedralSolid< dimension > >,       \
        "PolyhedralSolid" + std::to_string( dimension ) + "D" );               \
    PYTHON_FACTORY_CLASS( PolyhedralSolidInputFactory##dimension##D );         \
    PYTHON_FACTORY_CLASS( PolyhedralSolidOutputFactory##dimension##D )

namespace geode
{
    void define_polyhedral_solid_io( pybind11::module& module )
    {
        PYTHON_POLYHEDRAL_SOLID_IO( 3 );
    }
} // namespace geode
