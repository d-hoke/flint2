/*
    Copyright (C) 2018 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "nmod_mpoly.h"

void nmod_mpoly_randtest_bound(nmod_mpoly_t A, flint_rand_t state,
                     slong length, ulong exp_bound, const nmod_mpoly_ctx_t ctx)
{
    slong i, j, nvars = ctx->minfo->nvars;
    ulong * exp;
    TMP_INIT;

    TMP_START;
    exp = (ulong *) TMP_ALLOC(nvars*sizeof(ulong));

    nmod_mpoly_zero(A, ctx);
    for (i = 0; i < length; i++)
    {
        for (j = 0; j < nvars; j++)
            exp[j] = n_randint(state, exp_bound);

        _nmod_mpoly_emplacebackterm_ui_ui(A,
                               n_randint(state, ctx->ffinfo->mod.n), exp, ctx);
    }
    nmod_mpoly_sort_terms(A, ctx);
    nmod_mpoly_combine_like_terms(A, ctx);

    TMP_END;
}
