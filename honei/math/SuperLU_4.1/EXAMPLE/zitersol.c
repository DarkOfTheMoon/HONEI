
#include "slu_zdefs.h"
/*
 * -- SuperLU routine (version 4.1) --
 * Lawrence Berkeley National Laboratory
 * November, 2010
 */

int *GLOBAL_PERM_C, *GLOBAL_PERM_R;
SuperMatrix *GLOBAL_A, *GLOBAL_L, *GLOBAL_U;
SuperLUStat_t *GLOBAL_STAT;

void zmatvec_mult(doublecomplex alpha, doublecomplex x[], doublecomplex beta, doublecomplex y[])
{
    SuperMatrix *A = GLOBAL_A;

    sp_zgemv("N", alpha, A, x, 1, beta, y, 1);
}

void zpsolve(int n, doublecomplex x[], doublecomplex y[])
{
    extern void zcopy_(int *, doublecomplex [], int *, doublecomplex [], int *);

    int i_1 = 1;
    SuperMatrix *L = GLOBAL_L, *U = GLOBAL_U;
    SuperLUStat_t *stat = GLOBAL_STAT;
    int *perm_c = GLOBAL_PERM_C, *perm_r = GLOBAL_PERM_R;
    int info;
    static DNformat X;
    static SuperMatrix XX = {SLU_DN, SLU_Z, SLU_GE, 1, 1, &X};

    zcopy_(&n, y, &i_1, x, &i_1);
    XX.nrow = n;
    X.lda = n;
    X.nzval = x;
    zgstrs(NOTRANS, L, U, perm_c, perm_r, &XX, stat, &info);
}

int main(int argc, char *argv[])
{
    void zmatvec_mult(doublecomplex alpha, doublecomplex x[], doublecomplex beta, doublecomplex y[]);
    void zpsolve(int n, doublecomplex x[], doublecomplex y[]);
    extern int zfgmr( int n,
        void (*matvec_mult)(doublecomplex, doublecomplex [], doublecomplex, doublecomplex []),
        void (*psolve)(int n, doublecomplex [], doublecomplex[]),
        doublecomplex *rhs, doublecomplex *sol, double tol, int restrt, int *itmax,
        FILE *fits);
    extern int zfill_diag(int n, NCformat *Astore);

    char     equed[1] = {'B'};
    yes_no_t equil;
    trans_t  trans;
    SuperMatrix A, L, U;
    SuperMatrix B, X;
    NCformat *Astore;
    NCformat *Ustore;
    SCformat *Lstore;
    doublecomplex   *a;
    int      *asub, *xa;
    int      *etree;
    int      *perm_c; /* column permutation vector */
    int      *perm_r; /* row permutations from partial pivoting */
    int      nrhs, ldx, lwork, info, m, n, nnz;
    doublecomplex   *rhsb, *rhsx, *xact;
    doublecomplex   *work = NULL;
    double   *R, *C;
    double   u, rpg, rcond;
    doublecomplex zero = {0.0, 0.0};
    doublecomplex one = {1.0, 0.0};
    doublecomplex none = {-1.0, 0.0};
    mem_usage_t   mem_usage;
    superlu_options_t options;
    SuperLUStat_t stat;

    int restrt, iter, maxit, i;
    double resid;
    doublecomplex *x, *b;

#ifdef DEBUG
    extern int num_drop_L, num_drop_U;
#endif

#if ( DEBUGlevel>=1 )
    CHECK_MALLOC("Enter main()");
#endif

    /* Defaults */
    lwork = 0;
    nrhs  = 1;
    equil = YES;
    u     = 0.1; /* u=1.0 for complete factorization */
    trans = NOTRANS;

    /* Set the default input options:
        options.Fact = DOFACT;
        options.Equil = YES;
        options.ColPerm = COLAMD;
        options.DiagPivotThresh = 0.1; //different from complete LU
        options.Trans = NOTRANS;
        options.IterRefine = NOREFINE;
        options.SymmetricMode = NO;
        options.PivotGrowth = NO;
        options.ConditionNumber = NO;
        options.PrintStat = YES;
        options.RowPerm = LargeDiag;
        options.ILU_DropTol = 1e-4;
        options.ILU_FillTol = 1e-2;
        options.ILU_FillFactor = 10.0;
        options.ILU_DropRule = DROP_BASIC | DROP_AREA;
        options.ILU_Norm = INF_NORM;
        options.ILU_MILU = SILU;
     */
    ilu_set_default_options(&options);

    /* Modify the defaults. */
    options.PivotGrowth = YES;    /* Compute reciprocal pivot growth */
    options.ConditionNumber = YES;/* Compute reciprocal condition number */

    if ( lwork > 0 ) {
        work = SUPERLU_MALLOC(lwork);
        if ( !work ) ABORT("Malloc fails for work[].");
    }

    /* Read matrix A from a file in Harwell-Boeing format.*/
    if (argc < 2)
    {
        printf("Usage:\n%s [OPTION] < [INPUT] > [OUTPUT]\nOPTION:\n"
                "-h -hb:\n\t[INPUT] is a Harwell-Boeing format matrix.\n"
                "-r -rb:\n\t[INPUT] is a Rutherford-Boeing format matrix.\n"
                "-t -triplet:\n\t[INPUT] is a triplet format matrix.\n",
                argv[0]);
        return 0;
    }
    else
    {
        switch (argv[1][1])
        {
            case 'H':
            case 'h':
                printf("Input a Harwell-Boeing format matrix:\n");
                zreadhb(&m, &n, &nnz, &a, &asub, &xa);
                break;
            case 'R':
            case 'r':
                printf("Input a Rutherford-Boeing format matrix:\n");
                zreadrb(&m, &n, &nnz, &a, &asub, &xa);
                break;
            case 'T':
            case 't':
                printf("Input a triplet format matrix:\n");
                zreadtriple(&m, &n, &nnz, &a, &asub, &xa);
                break;
            default:
                printf("Unrecognized format.\n");
                return 0;
        }
    }

    zCreate_CompCol_Matrix(&A, m, n, nnz, a, asub, xa, SLU_NC, SLU_Z, SLU_GE);
    Astore = A.Store;
    zfill_diag(n, Astore);
    printf("Dimension %dx%d; # nonzeros %d\n", A.nrow, A.ncol, Astore->nnz);
    fflush(stdout);

    if ( !(rhsb = doublecomplexMalloc(m * nrhs)) ) ABORT("Malloc fails for rhsb[].");
    if ( !(rhsx = doublecomplexMalloc(m * nrhs)) ) ABORT("Malloc fails for rhsx[].");
    zCreate_Dense_Matrix(&B, m, nrhs, rhsb, m, SLU_DN, SLU_Z, SLU_GE);
    zCreate_Dense_Matrix(&X, m, nrhs, rhsx, m, SLU_DN, SLU_Z, SLU_GE);
    xact = doublecomplexMalloc(n * nrhs);
    ldx = n;
    zGenXtrue(n, nrhs, xact, ldx);
    zFillRHS(trans, nrhs, xact, ldx, &A, &B);

    if ( !(etree = intMalloc(n)) ) ABORT("Malloc fails for etree[].");
    if ( !(perm_r = intMalloc(m)) ) ABORT("Malloc fails for perm_r[].");
    if ( !(perm_c = intMalloc(n)) ) ABORT("Malloc fails for perm_c[].");
    if ( !(R = (double *) SUPERLU_MALLOC(A.nrow * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for R[].");
    if ( !(C = (double *) SUPERLU_MALLOC(A.ncol * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for C[].");

    info = 0;
#ifdef DEBUG
    num_drop_L = 0;
    num_drop_U = 0;
#endif

    /* Initialize the statistics variables. */
    StatInit(&stat);

    /* Compute the incomplete factorization and compute the condition number
       and pivot growth using dgsisx. */
    zgsisx(&options, &A, perm_c, perm_r, etree, equed, R, C, &L, &U, work,
           lwork, &B, &X, &rpg, &rcond, &mem_usage, &stat, &info);

    Lstore = (SCformat *) L.Store;
    Ustore = (NCformat *) U.Store;
    printf("zgsisx(): info %d\n", info);
    if (info > 0 || rcond < 1e-8 || rpg > 1e8)
        printf("WARNING: This preconditioner might be unstable.\n");

    if ( info == 0 || info == n+1 ) {

        if ( options.PivotGrowth == YES )
            printf("Recip. pivot growth = %e\n", rpg);
        if ( options.ConditionNumber == YES )
            printf("Recip. condition number = %e\n", rcond);

    } else if ( info > 0 && lwork == -1 ) {
        printf("** Estimated memory: %d bytes\n", info - n);
    }
    printf("n(A) = %d, nnz(A) = %d\n", n, Astore->nnz);
    printf("No of nonzeros in factor L = %d\n", Lstore->nnz);
    printf("No of nonzeros in factor U = %d\n", Ustore->nnz);
    printf("No of nonzeros in L+U = %d\n", Lstore->nnz + Ustore->nnz - n);
    printf("Fill ratio: nnz(F)/nnz(A) = %.3f\n",
            ((double)(Lstore->nnz) + (double)(Ustore->nnz) - (double)n)
            / (double)Astore->nnz);
    printf("L\\U MB %.3f\ttotal MB needed %.3f\n",
           mem_usage.for_lu/1e6, mem_usage.total_needed/1e6);
    fflush(stdout);

    /* Set the global variables. */
    GLOBAL_A = &A;
    GLOBAL_L = &L;
    GLOBAL_U = &U;
    GLOBAL_STAT = &stat;
    GLOBAL_PERM_C = perm_c;
    GLOBAL_PERM_R = perm_r;

    /* Set the variables used by GMRES. */
    restrt = SUPERLU_MIN(n / 3 + 1, 50);
    maxit = 1000;
    iter = maxit;
    resid = 1e-8;
    if (!(b = doublecomplexMalloc(m))) ABORT("Malloc fails for b[].");
    if (!(x = doublecomplexMalloc(n))) ABORT("Malloc fails for x[].");

    if (info <= n + 1)
    {
        int i_1 = 1;
        double maxferr = 0.0, nrmA, nrmB, res, t;
        doublecomplex temp;
        extern double dznrm2_(int *, doublecomplex [], int *);
        extern void zaxpy_(int *, doublecomplex *, doublecomplex [], int *, doublecomplex [], int *);

        /* Call GMRES. */
        for (i = 0; i < n; i++) b[i] = rhsb[i];
        for (i = 0; i < n; i++) x[i] = zero;

        t = SuperLU_timer_();

        zfgmr(n, zmatvec_mult, zpsolve, b, x, resid, restrt, &iter, stdout);

        t = SuperLU_timer_() - t;

        /* Output the result. */
        nrmA = dznrm2_(&(Astore->nnz), (doublecomplex *)((DNformat *)A.Store)->nzval,
                &i_1);
        nrmB = dznrm2_(&m, b, &i_1);
        sp_zgemv("N", none, &A, x, 1, one, b, 1);
        res = dznrm2_(&m, b, &i_1);
        resid = res / nrmB;
        printf("||A||_F = %.1e, ||B||_2 = %.1e, ||B-A*X||_2 = %.1e, "
                "relres = %.1e\n", nrmA, nrmB, res, resid);

        if (iter >= maxit)
        {
            if (resid >= 1.0) iter = -180;
            else if (resid > 1e-8) iter = -111;
        }
        printf("iteration: %d\nresidual: %.1e\nGMRES time: %.2f seconds.\n",
                iter, resid, t);

        /* Scale the solution back if equilibration was performed. */
        if (*equed == 'C' || *equed == 'B')
            for (i = 0; i < n; i++) zd_mult(&x[i], &x[i], C[i]);

        for (i = 0; i < m; i++) {
            z_sub(&temp, &x[i], &xact[i]);
            maxferr = SUPERLU_MAX(maxferr, z_abs1(&temp));
        }
        printf("||X-X_true||_oo = %.1e\n", maxferr);
    }
#ifdef DEBUG
    printf("%d entries in L and %d entries in U dropped.\n",
            num_drop_L, num_drop_U);
#endif
    fflush(stdout);

    if ( options.PrintStat ) StatPrint(&stat);
    StatFree(&stat);

    SUPERLU_FREE (rhsb);
    SUPERLU_FREE (rhsx);
    SUPERLU_FREE (xact);
    SUPERLU_FREE (etree);
    SUPERLU_FREE (perm_r);
    SUPERLU_FREE (perm_c);
    SUPERLU_FREE (R);
    SUPERLU_FREE (C);
    Destroy_CompCol_Matrix(&A);
    Destroy_SuperMatrix_Store(&B);
    Destroy_SuperMatrix_Store(&X);
    if ( lwork >= 0 ) {
        Destroy_SuperNode_Matrix(&L);
        Destroy_CompCol_Matrix(&U);
    }
    SUPERLU_FREE(b);
    SUPERLU_FREE(x);

#if ( DEBUGlevel>=1 )
    CHECK_MALLOC("Exit main()");
#endif

    return 0;
}
