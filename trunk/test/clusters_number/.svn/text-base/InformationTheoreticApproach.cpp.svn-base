#include <iostream>
#include <math.h>
#include <vector>
#include <assert.h>

#include <kvs/AnyValueArray>
#include <kvs/Matrix>
#include <kvs/TableObject>
#include <kvs/TableImporter>
#include <util/FastKMeansClustering.h>

#include "InformationTheoreticApproach.h"

namespace ita
{
    ITApproach::ITApproach()
    {
        // null
    }

    int ITApproach::exec(const kvs::TableObject& input, const int nclusters)
    {
        // initialize
        size_t nc = input.ncolumns();
        size_t nr = input.nrows();
        double y = nc/2.0;
        int n = nclusters;
        int output = 0;
        kvs::ValueArray<double> D(n+1);
        D[0] = 0;

        // calculate average Mahalanobis distance
        for (int k=1; k<n+1; k++)
        {
            kvsoceanvis::pcs::FastKMeansClustering* clustered_table = new kvsoceanvis::pcs::FastKMeansClustering();
            clustered_table -> setSeedingMethod(kvsoceanvis::pcs::FastKMeansClustering::SmartSeeding);
            clustered_table -> setNumberOfClusters(k);
            clustered_table -> exec(&input);
            kvs::ValueArray<double> x(nc);
            kvs::ValueArray<double> cx(nc);
            kvs::ValueArray<double> covar(nc*nc);

            // set the common covariance matrix as the identity matrix
            for (size_t j=0; j<nc; j++)
            {
                for (size_t i=0; i<nc; i++)
                {
                    if (i == j)
                    {
                        covar[i + j*nc] = 1;
                    }
                    else
                    {
                        covar[i + j*nc] = 0;
                    }
                }
            }
            double d = 0;
            for (size_t j=0; j<nr; j++)
            {
                double dist = 0;
                for (size_t i=0; i<nc; i++)
                {
                    cx[i] = clustered_table->center(0)[i];
                }
                x = ITApproach::GetRowArray(&input, j);
                dist = MahalanobisDistance(nc, x, cx, covar);
                for (int i=1; i<k; i++)
                {
                    double tmp = 0;
                    for (size_t p=0; p<nc; p++)
                    {
                        cx[p] = clustered_table->center(i)[p];
                    }
                    x = ITApproach::GetRowArray(&input, j);
                    tmp = MahalanobisDistance(nc, x, cx, covar);
                    if (tmp < dist)
                    {
                        dist = tmp;
                    }
                }
                d += dist;
            }
            d = (1.0/nc) * (1.0/nr) * d;
            D[k] = pow(d, -y);
            std::cout << D[k] << std::endl;
            delete clustered_table;
        }

        // calculate maximal jump
        double jump = D[1] - D[0];
        output = 1;
        for (int i=1; i<n; i++)
        {
            double tmp = D[i+1] - D[i];
            if (tmp > jump)
            {
                jump = tmp;
                output = i+1;
            }
        }
        return output;
    }

    kvs::ValueArray<double> ITApproach::GetRowArray(const kvs::TableObject* table, const size_t i )
    {
        kvs::ValueArray<double> row( table->ncolumns() );
        const size_t ncolumns = table->ncolumns();
        for ( size_t j = 0; j < ncolumns; j++ )
        {
            row[j] = table->column(j).to<double>(i);
        }

        return row;
    }

    double ITApproach::MahalanobisDistance(const size_t dim, const kvs::ValueArray<double>& x, const kvs::ValueArray<double>& cx, const kvs::ValueArray<double>& gamma)
    {
        double output = 0;
        kvs::Matrix<double> tmp(1, dim);
        kvs::Matrix<double> tmp2(1, dim);
        kvs::Matrix<double> Gamma(dim, dim);
        for (size_t j=0; j<dim; j++)
        {
            tmp[0][j] = x[j] - cx[j];
            for (size_t i=0; i<dim; i++)
            {
                Gamma[j][i] = gamma[i + j*dim];
            }
        }
        tmp2 = tmp;
        tmp2.transpose();
        output = (tmp * Gamma.inverse() * tmp2)[0][0];

        return output;
    }
}
