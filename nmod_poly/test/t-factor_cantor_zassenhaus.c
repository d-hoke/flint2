/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2007 David Howden
    Copyright (C) 2007, 2008, 2009, 2010 William Hart
    Copyright (C) 2008 Richard Howell-Peak
    Copyright (C) 2011 Fredrik Johansson

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpir.h>
#include "flint.h"
#include "nmod_vec.h"
#include "nmod_poly.h"
#include "ulong_extras.h"

int
main(void)
{
    int iter;
    flint_rand_t state;
    flint_randinit(state);

    printf("factor_cantor_zassenhaus....");
    fflush(stdout);

    for (iter = 0; iter < 200; iter++)
    {
        int result = 1;
        nmod_poly_t pol1, poly, quot, rem;
        nmod_poly_t product;
        nmod_poly_factor_t res;
        mp_limb_t modulus, lead;
        long i, j, length, num_factors;
        ulong prod1, exponents[5];

        do { modulus = n_randtest_prime(state, 0); } while (modulus <= 2);

        nmod_poly_init(pol1, modulus);
        nmod_poly_init(poly, modulus);
        nmod_poly_init(quot, modulus);
        nmod_poly_init(rem, modulus);

        nmod_poly_zero(pol1);
        nmod_poly_set_coeff_ui(pol1, 0, 1);

        length = n_randint(state, 7) + 2;

        do
        {
            nmod_poly_randtest(poly, state, length); 
            nmod_poly_make_monic(poly, poly);
        }
        while ((!nmod_poly_is_irreducible(poly)) || (poly->length < 2));

        exponents[0] = n_randint(state, 30) + 1;
        prod1 = exponents[0];
        for (i = 0; i < exponents[0]; i++)
            nmod_poly_mul(pol1, pol1, poly);

        num_factors = n_randint(state, 5) + 1;

        for (i = 1; i < num_factors; i++)
        {
            do 
            {
                length = n_randint(state, 7) + 2;
                nmod_poly_randtest(poly, state, length); 
                nmod_poly_make_monic(poly, poly);
                if (poly->length)
                    nmod_poly_divrem(quot, rem, pol1, poly);
            }
            while ((!nmod_poly_is_irreducible(poly)) || (poly->length < 2) ||
                (rem->length == 0));

            exponents[i] = n_randint(state, 30) + 1;
            prod1 *= exponents[i];
            for (j = 0; j < exponents[i]; j++)
                nmod_poly_mul(pol1, pol1, poly);
        }

        nmod_poly_factor_init(res);
        nmod_poly_factor_cantor_zassenhaus(res, pol1);
        result &= (res->num_factors == num_factors);

        if (!result)
        {
            printf("Error: number of factors incorrect, %ld, %ld\n",
                res->num_factors, num_factors);
        }

        nmod_poly_init(product, pol1->mod.n);
        nmod_poly_set_coeff_ui(product, 0, 1);
        for (i = 0; i < res->num_factors; i++)
            for (j = 0; j < res->exponents[i]; j++)
                nmod_poly_mul(product, product, res->factors[i]);

        lead = pol1->coeffs[pol1->length - 1];
        nmod_poly_scalar_mul_nmod(product, product, lead);
        result &= nmod_poly_equal(pol1, product);

        if (!result)
        {
            printf("Error: product of factors does not equal original polynomial\n");
            nmod_poly_print(pol1); printf("\n");
            nmod_poly_print(product); printf("\n");
        }

        if (!result)
            abort();


        nmod_poly_clear(product);
        nmod_poly_clear(quot);
        nmod_poly_clear(rem);
        nmod_poly_clear(pol1);
        nmod_poly_clear(poly);
        nmod_poly_factor_clear(res);
    }

    flint_randclear(state);
    printf("PASS\n");
    return 0;
}