#ifndef KCLUSTER_H_INCLUDED
#define KCLUSTER_H_INCLUDED

#include <stdlib.h>
#include <math.h>
#include <vector>
#include <time.h>
#include <assert.h>

#include <kvs/AnyValueArray>
#include <kvs/TableObject>
#include <kvs/MersenneTwister>

namespace kvs
{
class KCluster
{
public:

    KCluster();
    KCluster(int clusters);
    void setClusters(int clusters);
    kvs::TableObject exec();
    kvs::TableObject exec(const kvs::TableObject& table);
    kvs::ValueArray<int> getClusterIDs();

private:

    kvs::ValueArray<int> m_ClusterIDs;
    kvs::TableObject m_output;
    int m_nr; // number of rows
    int m_nc; // number of columns
    int m_k; // number of clusters

    void hspkcluster(const kvs::TableObject& input, const int nr, const int nc, const int k, kvs::ValueArray<int>& output);
    void PointAllCtrs(const kvs::TableObject& x, const std::vector<double>& c, std::vector<int>& a, std::vector<double>& u, std::vector<double>& l, const int nr, const int nc, const int k, const int m);
    kvs::Real64 GetValue(const kvs::AnyValueArray& array, const size_t index);
    double GetDist(const std::vector<double>& x1, const std::vector<double>& x2);
    void GetPoint(const kvs::TableObject& source, const int nr, const int nc, const int idx, std::vector<double>& target);
    void GetPoint(const std::vector<double>& source, const int nr, const int nc, const int idx, std::vector<double>& target);
    int UpdateNap(const std::vector<int>& a, const int nr, const int m);
    void UpdateVsum(const kvs::TableObject& x, const std::vector<int>& a, const int nr, const int nc, const int m, std::vector<double>& cs);
    void MoveCenters(const std::vector<double>& cs, const std::vector<int>& q, std::vector<double>& c, std::vector<double>& p, const int k, const int nc);
    void UpdateBounds(const std::vector<double>& p, const std::vector<int>& a, std::vector<double>& u, std::vector<double>& l, const int nr, const int k);
    kvs::TableObject CreateRandomTable(const int nrows, const int ncolumns);
};
}

#endif // KCLUSTER_H_INCLUDED
