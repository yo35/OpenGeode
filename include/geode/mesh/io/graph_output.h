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

#pragma once

#include <absl/strings/string_view.h>

#include <geode/basic/factory.h>
#include <geode/basic/output.h>

#include <geode/mesh/common.h>

namespace geode
{
    class Graph;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a Graph.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] graph Graph to save.
     * @param[in] filename Path to the file where save the Graph.
     */
    std::vector< std::string > opengeode_mesh_api save_graph(
        const Graph& graph, absl::string_view filename );

    class GraphOutput : public Output< Graph >
    {
    protected:
        GraphOutput( absl::string_view filename ) : Output< Graph >{ filename }
        {
        }
    };

    bool opengeode_mesh_api is_graph_saveable(
        const Graph& graph, absl::string_view filename );

    using GraphOutputFactory =
        Factory< std::string, GraphOutput, absl::string_view >;
} // namespace geode
