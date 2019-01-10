#include "KCluster.h"

namespace kvs
{
KCluster::KCluster()
{
    m_k = 3;
}

KCluster::KCluster(int clusters)
{
    m_k = clusters;
}

void KCluster::setClusters(int clusters)
{
    m_k = clusters;
}

kvs::TableObject KCluster::exec(const kvs::TableObject& table)
{
    m_nr = table.column(0).size();
    m_nc = table.ncolumns();

    m_ClusterIDs.allocate(m_nr);
    KCluster::hspkcluster(table, m_nr, m_nc, m_k, m_ClusterIDs);
    m_output = table;
    m_output.addColumn(m_ClusterIDs);
    return m_output;
}

kvs::TableObject KCluster::exec()
{
    kvs::TableObject table = KCluster::CreateRandomTable(500, 5);
    m_nr = table.column(0).size();
    m_nc = table.ncolumns();

    m_ClusterIDs.allocate(m_nr);
    KCluster::hspkcluster(table, m_nr, m_nc, m_k, m_ClusterIDs);
    m_output = table;
    m_output.addColumn(m_ClusterIDs);
    return m_output;
}

kvs::ValueArray<int> KCluster::getClusterIDs()
{
    return m_ClusterIDs;
}
        
void KCluster::hspkcluster(const kvs::TableObject& input, const int nr, const int nc, const int k, kvs::ValueArray<int>& output)
{
    // initialization

    std::vector<double> center(k*nc); // cluster center
    std::vector<int> nap(k); // number of the points assigned to cluster n
    std::vector<double> vsum(k*nc); // vector sum of all points in cluster n
    std::vector<int> aindex(nr); // index of the center to which x(m) is assigned
    std::vector<double> lbound(nr); // lower bound on the distance between x(m) and its second closest center (the closest center to x(m) that is not c(a(m)))
    std::vector<double> ubound(nr); // upper bound on the distance between x(m) and its assigned center c(a(m))
    std::vector<double> sdist(k); // distance from c(n) to its closest other center
    std::vector<double> pdist(k); // distance that c(n) last moved

    // randomly assign center points

    srand(time(NULL));
    int tmp1 = nr/k;
    int tmp2 = rand()%tmp1;
    for(int i=0; i<k; i++)
    {
        for(int j=0; j<nc; j++)
        {
            center[i*nc+j] = GetValue(input.column(j), tmp2);
        }
        tmp2 = tmp2 + tmp1;
    }

    // initialization (point-all-ctrs)

    for (int i=0; i<nr; i++)
    {
        KCluster::PointAllCtrs(input, center, aindex, ubound, lbound, nr, nc, k, i);
        // calculate nap
        nap[aindex[i]] = nap[aindex[i]] + 1;
        // calculate vsum
        for (int j=0; j<nc; j++)
        {
            vsum[nc*aindex[i]+j] = vsum[nc*aindex[i]+j] + GetValue(input.column(j), i);
        }
    }

    // clustering algorithm

    std::vector<double> tmpc1(nc);
    std::vector<double> tmpc2(nc);
    std::vector<double> tmpcm(nc);
    std::vector<double> tmpcn(nc);

    std::vector<double> tmpc(nc);
    std::vector<double> tmpx(nc);

    bool converged = false;
    while (converged != true)
    {
        for (int i=0; i<k; i++)
        {
            double d1;
            double d2;
            KCluster::GetPoint(center, k, nc, 0, tmpc1);
            KCluster::GetPoint(center, k, nc, 1, tmpc2);
            KCluster::GetPoint(center, k, nc, i, tmpcm);
            if (i == 0)
            {
                d1 = KCluster::GetDist(tmpc1, tmpc2);
            }
            else
            {
                d1 = KCluster::GetDist(tmpc1, tmpcm);
            }
            for (int j=0; j<k; j++)
            {
                if (j != i)
                {
                    KCluster::GetPoint(center, k, nc, j, tmpcn);
                    d2 = KCluster::GetDist(tmpcm, tmpcn);
                    if (d2<d1)
                    {
                        d1 = d2;
                    }
                }
            }
            sdist[i] = d1;
        }

        for (int i=0; i<nr; i++)
        {
            double m;
            m = (sdist[aindex[i]]/4.0 > lbound[i]) ? sdist[aindex[i]]/4.0 : lbound[i];
            if (ubound[i] > m)
            {
                KCluster::GetPoint(center, k, nc, aindex[i], tmpc);
                KCluster::GetPoint(input, nr, nc, i, tmpx);
                ubound[i] = KCluster::GetDist(tmpc, tmpx);
                if (ubound[i] > m)
                {
                    int aidx = aindex[i];
                    KCluster::PointAllCtrs(input, center, aindex, ubound, lbound, nr, nc, k, i);
                    if (aidx != aindex[i])
                    {
                        nap[aidx] = KCluster::UpdateNap(aindex, nr, aidx);
                        nap[aindex[i]] = KCluster::UpdateNap(aindex, nr, aindex[i]);
                        KCluster::UpdateVsum(input, aindex, nr, nc, aidx, vsum);
                        KCluster::UpdateVsum(input, aindex, nr, nc, aindex[i], vsum);
                    }
                }
            }
        }

        //move centers

        KCluster::MoveCenters(vsum, nap, center, pdist, k, nc);

        // update bounds

        KCluster::UpdateBounds(pdist, aindex, ubound, lbound, nr, k);

        // converged condition

        int count = 0;
        for (int i=0; i<k; i++)
        {
            if (pdist[i] == 0)
            {
                count = count + 1;
            }
        }
        if (count == k)
        {
            for (int i=0; i<nr; i++)
            {
                output[i] = aindex[i];
            }
            converged = true;
        }
    }
}

void KCluster::PointAllCtrs(const kvs::TableObject& x, const std::vector<double>& c, std::vector<int>& a, std::vector<double>& u, std::vector<double>& l, const int nr, const int nc, const int k, const int m)
{
    std::vector<double> tmpx(nc);
    std::vector<double> tmpc1(nc);
    std::vector<double> tmpc(nc);
    double d1, d2, d3, d4;
    KCluster::GetPoint(x, nr, nc, m, tmpx);
    KCluster::GetPoint(c, k, nc, 0, tmpc1);
    d1 = KCluster::GetDist(tmpx, tmpc1);
    a[m] = 0;
    for (int i=0; i<k; i++)
    {
        KCluster::GetPoint(c, k, nc, i, tmpc);
        d3 = KCluster::GetDist(tmpx, tmpc);
        if (d3 < d1)
        {
            d1 = d3;
            a[m] = i;
        }
    }
    KCluster::GetPoint(c, k, nc, a[m], tmpc);
    u[m] = KCluster::GetDist(tmpx, tmpc);
    std::vector<double> tmpc2(nc);
    KCluster::GetPoint(c, k, nc, 1, tmpc2);
    if (a[m] != 0)
    {
        d2 = KCluster::GetDist(tmpx, tmpc1);
    }
    else
    {
        d2 = KCluster::GetDist(tmpx, tmpc2);
    }
    for (int i=0; i<k; i++)
    {
        KCluster::GetPoint(c, k, nc, i, tmpc);
        if (i != a[m])
        {
            d4 = KCluster::GetDist(tmpx, tmpc);
            if (d4 < d2)
            {
                d2 = d4;
            }
        }
    }
    l[m] = d2;
}

kvs::Real64 KCluster::GetValue(const kvs::AnyValueArray& array, const size_t index)
{
    const std::type_info& type = array.typeInfo()->type();
    if ( type == typeid( kvs::Int8   ) ) return( static_cast<kvs::Real64>( array.at<kvs::Int8>(index) ) );
    if ( type == typeid( kvs::Int16  ) ) return( static_cast<kvs::Real64>( array.at<kvs::Int16>(index) ) );
    if ( type == typeid( kvs::Int32  ) ) return( static_cast<kvs::Real64>( array.at<kvs::Int32>(index) ) );
    if ( type == typeid( kvs::Int64  ) ) return( static_cast<kvs::Real64>( array.at<kvs::Int64>(index) ) );
    if ( type == typeid( kvs::UInt8  ) ) return( static_cast<kvs::Real64>( array.at<kvs::UInt8>(index) ) );
    if ( type == typeid( kvs::UInt16 ) ) return( static_cast<kvs::Real64>( array.at<kvs::UInt16>(index) ) );
    if ( type == typeid( kvs::UInt32 ) ) return( static_cast<kvs::Real64>( array.at<kvs::UInt32>(index) ) );
    if ( type == typeid( kvs::UInt64 ) ) return( static_cast<kvs::Real64>( array.at<kvs::UInt64>(index) ) );
    if ( type == typeid( kvs::Real32 ) ) return( static_cast<kvs::Real64>( array.at<kvs::Real32>(index) ) );
    if ( type == typeid( kvs::Real64 ) ) return( static_cast<kvs::Real64>( array.at<kvs::Real64>(index) ) );

    kvsMessageError("Unsupported data type.");
    return( 0.0 );
}       
        
void KCluster::GetPoint(const kvs::TableObject& source, const int nr, const int nc, const int idx, std::vector<double>& target)
{
    assert(idx < nr);
    for (int i=0; i<nc; i++)
    {
        target[i] = GetValue(source.column(i), idx);
    }
}

void KCluster::GetPoint(const std::vector<double>& source, const int nr, const int nc, const int idx, std::vector<double>& target)
{
    assert(idx < nr);
    for (int i=0; i<nc; i++)
    {
        target[i] = source[nc*idx+i];
    }
}

double KCluster::GetDist(const std::vector<double>& x1, const std::vector<double>& x2)
{
    assert(x1.size() == x2.size());
    double sum = 0.0;
    double temp = 0.0;
    for(size_t i=0; i<x1.size(); i++)
    {
        temp = fabs(x1[i] - x2[i]);
        sum += temp*temp;
    }
    return sum;
}

int KCluster::UpdateNap(const std::vector<int>& a, const int nr, const int m)
{
    int q = 0;
    for (int i=0; i<nr; i++)
    {
        if (a[i] == m)
        {
            q = q + 1;
        }
    }
    return q;
}

void KCluster::UpdateVsum(const kvs::TableObject& x, const std::vector<int>& a, const int nr, const int nc, const int m, std::vector<double>& cs)
{
    for (int i=0; i<nc; i++)
    {
        cs[nc*m+i] = 0;
    }
    for (int i=0; i<nr; i++)
    {
        if (a[i] == m)
        {
            for (int j=0; j<nc; j++)
            {
                double tmp = KCluster::GetValue(x.column(j), i);
                cs[nc*m+j] = cs[nc*m+j] + tmp;
            }
        }
    }
}

void KCluster::MoveCenters(const std::vector<double>& cs, const std::vector<int>& q, std::vector<double>& c, std::vector<double>& p, const int k, const int nc)
{
    std::vector<double> c1(nc);
    std::vector<double> tmpc(nc);
    for (int i=0; i<k; i++)
    {
        for (int j=0; j<nc; j++)
        {
            c1[j] = c[i*nc+j];
        }
        assert(q[i] != 0);
        for (int j=0; j<nc; j++)
        {
            c[i*nc+j] = cs[i*nc+j]/q[i];
        }
        KCluster::GetPoint(c, k, nc, i, tmpc);
        p[i] = KCluster::GetDist(c1, tmpc);
    }
}

void KCluster::UpdateBounds(const std::vector<double>& p, const std::vector<int>& a, std::vector<double>& u, std::vector<double>& l, const int nr, const int k)
{
    double d1 = p[0];
    double d2 = p[0];
    int r = 0;
    int r1 = 0;
    for (int i=0; i<k; i++)
    {
        if (p[i]>d1)
        {
            d1 = p[i];
            r = i;
        }
    }
    if (r == 0)
    {
        r1 = 1;
        d2 = p[1];
    }
    for (int i=0; i<k; i++)
    {
        if (i != r && p[i]>d2)
        {
            d2 = p[i];
            r1 = i;
        }
    }
    for (int i=0; i<nr; i++)
    {
        u[i] = u[i] + p[a[i]];
        if (r == a[i])
        {
            l[i] = l[i] - p[r1];
        }
        else
        {
            l[i] = l[i] - p[r];
        }
    }
}

kvs::TableObject KCluster::CreateRandomTable(const int nrows, const int ncolumns)
{
    kvs::TableObject table;

    kvs::MersenneTwister tmp;
    for ( int i = 0; i < ncolumns; i++ )
    {
        kvs::ValueArray<kvs::Int32> array( nrows );
        for ( int j = 0; j < nrows; j++ )
        {
            array[j] = static_cast<kvs::Int32>( 100 * tmp.rand() );
        }

        table.addColumn( array );
    }

    return( table );
}
}
