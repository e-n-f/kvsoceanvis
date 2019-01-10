#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <assert.h>

#include <kvs/AnyValueArray>
#include <kvs/TableObject>
#include <kvs/Timer>
extern"C"
{
    #include "cblas.h"  
    #include "lapacke.h"  
}


#include "MDSTable.h"

namespace mds
{
    MDSTable::MDSTable()
    {
        // null
    }

    kvs::TableObject MDSTable::exec(const kvs::TableObject& input)
    {
        m_nrows = input.nrows();
        double* dsmatrix = new double[m_nrows*m_nrows];
        MDSTable::DSMatrix(input, dsmatrix);
        double* ipmatrix = new double[m_nrows*m_nrows];
        MDSTable::IPMatrix(dsmatrix, ipmatrix);
        kvs::TableObject output;
        output = OutputTable(ipmatrix);
        delete []dsmatrix;
        delete []ipmatrix;
        return output;
    }

    kvs::TableObject MDSTable::exec(const double input[], size_t n)
    {
        m_nrows = n;
        double* ipmatrix = new double[m_nrows*m_nrows];
        MDSTable::IPMatrix(input, ipmatrix);
        kvs::TableObject output;
        output = OutputTable(ipmatrix);
        delete []ipmatrix;
        return output;
    }

    kvs::ValueArray<kvs::Real32> MDSTable::GetRowArray(const kvs::TableObject* table, const size_t i)
    {
        kvs::ValueArray<kvs::Real32> row( table->ncolumns() );
        const size_t ncolumns = table->ncolumns();
        for ( size_t j = 0; j < ncolumns; j++ )
        {
            row[j] = table->column(j).to<kvs::Real32>(i);
        }
        return row;
    }

    kvs::Real32 MDSTable::GetEuclideanDistance(const kvs::ValueArray<kvs::Real32>& x0, const kvs::ValueArray<kvs::Real32>& x1)
    {
        kvs::Real32 distance = 0.0f;
        const size_t nrows = x0.size();
        for ( size_t i = 0; i < nrows; i++ )
        {
            const kvs::Real32 diff = x1[i] - x0[i];
            distance += diff * diff;
        }
        return distance;
    }

    void MDSTable::DSMatrix(const kvs::TableObject& input, double output[])
    {
        size_t n = m_nrows;
        memset(output, 0, n*n*sizeof(double));
        for (size_t i=1; i<n; i++)
        {
            for (size_t j=0; j<i; j++)
            {
                output[i*n+j] = sqrt(MDSTable::GetEuclideanDistance(MDSTable::GetRowArray(&input, i), MDSTable::GetRowArray(&input, j)));
                output[j*n+i] = output[i*n+j];
            }
        }
    }

    void MDSTable::IPMatrix(const double input[], double output[])
    {
        kvs::Timer timer;
        timer.start();
        size_t n = m_nrows;
        double* identify = new double[n*n];
        double* one = new double[n*n];
        double* dsq = new double[n*n];
        double* center = new double[n*n];
        double* tmp = new double[n*n];
        memset(identify, 0, n*n*sizeof(double));
        memset(one, 1, n*n*sizeof(double));
        memset(dsq, 0, n*n*sizeof(double));
        memset(center, 0, n*n*sizeof(double));
        memset(tmp, 0, n*n*sizeof(double));
        memset(output, 0, n*n*sizeof(double));
        for (size_t i=0; i<n; i++)
        {
            identify[i*n+i] = 1;
        }
        for (size_t i=0; i<n*n; i++)
        {
            center[i] = identify[i] - (1.0/n)*one[i];
            dsq[i] = input[i]*input[i];
        }
        const enum CBLAS_ORDER Order=CblasRowMajor;
        const enum CBLAS_TRANSPOSE TransA=CblasNoTrans;
        const enum CBLAS_TRANSPOSE TransB=CblasNoTrans;
        cblas_dgemm(Order, TransA, TransB, n, n, n, 1, center, n, dsq, n, 0, tmp, n);
        cblas_dgemm(Order, TransA, TransB, n, n, n, 1, tmp, n, center, n, 0, output, n);
        for (size_t i=0; i<n*n; i++)
        {
            output[i] = output[i]*(-0.5);
        }
        timer.stop();
        std::cout << "Inner Product Matrix: " << timer.msec() << " [msec]" << std::endl;
        delete []identify;
        delete []one;
        delete []dsq;
        delete []center;
        delete []tmp;
    }

    kvs::TableObject MDSTable::OutputTable(double input[])
    {
        kvs::TableObject output;
        size_t n = m_nrows;
        double* wr = new double[n];
        double* wi = new double[n];
        double* vl = new double[n*n];
        double* vr = new double[n*n];
        memset(wr, 0, n*sizeof(double));
        memset(wi, 0, n*sizeof(double));
        memset(vl, 0, n*n*sizeof(double));
        memset(vr, 0, n*n*sizeof(double));
        int info = LAPACKE_dgeev(LAPACK_ROW_MAJOR, 'N', 'V', n, input, n, wr, wi, vl, n, vr, n);
        if (info != 0)
        {
            std::cout << "Error Code = " << info << std::endl;
        }
        double evalue0 = 0;
        double evalue1 = 0;
        size_t idx0 = 0;
        size_t idx1 = 0;
        for (size_t i=0; i<n; i++)
        {
            if (wr[i] > evalue0 && wi[i] == 0)
            {
                evalue0 = wr[i];
                idx0 = i;
            }
        }
        assert(evalue0 > 0);
        for (size_t i=0; i<n; i++)
        {
            if (wr[i] > evalue1 && wr[i] < evalue0 && wi[i] == 0)
            {
                evalue1 = wr[i];
                idx1 = i;
            }
        }
        assert(evalue1 > 0);
        assert(idx0 != idx1);
        double Lambda[4] = {sqrt(evalue0), 0, 0, sqrt(evalue1)};
        double* evector0 = new double[n];
        double* evector1 = new double[n];
        double dist0 = 0;
        double dist1 = 0;
        for (size_t i=0; i<n; i++)
        {
            evector0[i] = vr[n*i+idx0];
            dist0 += evector0[i]*evector0[i];
            evector1[i] = vr[n*i+idx1];
            dist1 += evector1[i]*evector1[i];
        }
        for (size_t i=0; i<n; i++)
        {
            evector0[i] = evector0[i]/sqrt(dist0);
            evector1[i] = evector1[i]/sqrt(dist1);
        }
        double* VMatrix = new double[2*n];
        for (size_t i=0; i<n; i++)
        {
            VMatrix[2*i] = evector0[i];
            VMatrix[2*i+1] = evector1[i];
        }
        double* XMatrix = new double[2*n];
        for (size_t i=0; i<2*n; i++)
        {
            XMatrix[i] = 0;
        }
        delete []wr;
        delete []wi;
        delete []vl;
        delete []vr;
        delete []evector0;
        delete []evector1;

        const enum CBLAS_ORDER Order=CblasRowMajor;
        const enum CBLAS_TRANSPOSE TransA=CblasNoTrans;
        const enum CBLAS_TRANSPOSE TransB=CblasNoTrans;
        cblas_dgemm(Order, TransA, TransB, n, 2, 2, 1, VMatrix, 2, Lambda, 2, 0, XMatrix, 2);

        kvs::ValueArray<float> x(n);
        kvs::ValueArray<float> y(n);
        for (size_t i=0; i<n; i++)
        {
            x[i] = XMatrix[2*i];
            y[i] = XMatrix[2*i+1];
        }
        output.addColumn(kvs::AnyValueArray(x));
        output.addColumn(kvs::AnyValueArray(y));
        delete []VMatrix;
        delete []XMatrix;
        return output;
    }
}
