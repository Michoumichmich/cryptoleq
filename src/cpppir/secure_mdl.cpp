// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: secure_mdl.cpp

#include "secure_int.h"
#include "open_int.h"

template<Compiler &M>
secure_int<M> secure_mp1(secure_int<M> A) {
    if (0) // old impl
    {
        // decrypting A

        // taking to phi
        Unumber x(M.access_phi());

        secure_int<M> mp1;

        // this must be a code generated by a compiler
        while (x != 0) {
            if (x.getbit(0)) mp1 += A;
            A += A;
            x >>= 1;
        }

        // taking to p1Nk1N
        x = M.access_p1Nk1N();

        A = mp1;
        mp1 = secure_int<M>(1);

        // this must be a code generated by a compiler
        while (x != 0) {
            if (x.getbit(0)) mp1 += A;
            A += A;
            x >>= 1;
        }

        return mp1;
    } else // new impl
    {
        Unumber x(M.fkf());

        secure_int<M> mp1;

        // this must be a code generated by a compiler
        while (x != 0) {
            if (x.getbit(0)) mp1 += A;
            A += A;
            x >>= 1;
        }

        return mp1;
    }
}


template<Compiler &M>
secure_int<M> secure_mdl(secure_int<M> A, secure_int<M> B) {
    // if Decrypted(A).gt0() return B*rand
    // othrwise return rand

    // decrypting A

    secure_int<M> mp1 = secure_mp1<M>(A);

    Unumber rN = M.random(); // rN.pow( M.N, M.N2 );
    // M.random returns r^N, so no powering required

    secure_int<M> r(rN);

    if (mp1.gt0()) r += B;

    return r;
}

template<Compiler &M>
void extract_rm(secure_int<M> x, Unumber &r, Unumber &m) {
    secure_int<M> d = secure_mp1<M>(x); // decrypting

    m = d.X() - 1;
    if (m % M.proc.N != 0) throw "secure_int<M>::show - bad value";

    m = m / M.proc.N;

    // m is found, now calculate r
    // first get r^N by r^N*(1+Nkm) * (1-Nkm)
    // (1-Nkm) = g^(N-m);

    r = M.access_g();

    r.pow(base_int<M>::congruenceN(M.proc.N - m), M.proc.N2);
    r = r.mul(x.X(), M.proc.N2);
    r.pow(M.access_Nm1(), M.proc.N);

    // verify

    Unumber rN = r;
    rN.pow(M.proc.N, M.proc.N2);
    Unumber y = M.access_g();
    y.pow(m, M.proc.N2);

    y = y.mul(rN, M.proc.N2);

    if (x.X() != y)
        throw "extract_rm - verification failed";
}

template secure_int<compiler>
secure_mdl<compiler>(secure_int<compiler>, secure_int<compiler>);

template void
extract_rm<compiler>(secure_int<compiler>, Unumber &r, Unumber &m);



