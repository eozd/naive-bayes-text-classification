/*
 * Copyright 2018 Esref Ozdemir
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "defs.hpp"

namespace ir {
/**
 * @brief Convert HTML special character sequences to their readable text
 * equivalents.
 *
 * This function converts special HTML character sequences such as &lt; to their
 * ASCII equivalents. Since a given raw document may possibly be large, the
 * sequences are modified in-place and no string copy is made.
 *
 * Since each converted HTML sequence corresponds to a single ASCII character,
 * only the last element of the sequence is replaced by the ASCII character and
 * the rest is replaced with space character.
 *
 * Currently, the replaced character sequences and their replace value is as
 * follows:
 *
 *
 * HTML Sequence | Replace Character
 * ------------- | -----------------
 *     "&#1;"	 |        ' '
 *     "&#2;"	 |        ' '
 *     "&#3;"	 |        ' '
 *     "&#5;"	 |    ASCII code 5
 *     "&#22;"	 |        ' '
 *     "&#27;"	 |        ' '
 *     "&#30;"	 |    ASCII code 30
 *     "&#31;"	 |    ASCII code 31
 *     "&#127;"	 |        ' '
 *     "&amp;"	 |        '&'
 *     "&lt;"	 |        '<'
 *     "&gt;"	 |        '>'
 *
 * @param doc Raw document.
 */
void convert_html_special_chars(raw_doc& doc);
} // namespace ir
