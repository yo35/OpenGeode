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

#include <mutex>

#include <geode/model/common.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/vertex_identifier.h>

namespace geode
{
    class TopologyBuilder;
} // namespace geode

namespace geode
{
    /*!
     * This mixin stores topological information about a set of components.
     * This information is topological relationships between components and
     * identifications between geometric component vertices.
     */
    class opengeode_model_api Topology : public Relationships,
                                         public VertexIdentifier
    {
    protected:
        Topology() = default;
        Topology( Topology&& other ) = default;
        Topology& operator=( Topology&& other ) = default;
    };
} // namespace geode
