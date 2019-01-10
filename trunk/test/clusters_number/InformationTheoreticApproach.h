#ifndef INFORMATIONTHEORETICAPPROACH_H_INCLUDED
#define INFORMATIONTHEORETICAPPROACH_H_INCLUDED

namespace ita
{
class ITApproach
{
public:

    ITApproach();
    int exec(const kvs::TableObject& input, const int nclusters);

private:

    kvs::ValueArray<double> GetRowArray(const kvs::TableObject* table, const size_t i);
    double MahalanobisDistance(const size_t dim, const kvs::ValueArray<double>& x, const kvs::ValueArray<double>& cx, const kvs::ValueArray<double>& gamma);

};
}
#endif // INFORMATIONTHEORETICAPPROACH_H_INCLUDED
