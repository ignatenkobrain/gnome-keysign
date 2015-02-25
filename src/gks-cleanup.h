/* gks-cleanup.h
 *
 * Copyright (C) 2015 Igor Gnatenko <i.gnatenko.brain@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GKS_CLEANUP_H
#define GKS_CLEANUP_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GS_DEFINE_CLEANUP_FUNCTION0(Type, name, func) \
  static inline void name (void *v) \
  { \
    if (*(Type*)v) \
      func (*(Type*)v); \
  }

GS_DEFINE_CLEANUP_FUNCTION0(GError*, gs_local_free_error, g_error_free)

#define _cleanup_error_free_ __attribute__ ((cleanup(gs_local_free_error)))

G_END_DECLS

#endif /* GKS_CLEANUP_H */
