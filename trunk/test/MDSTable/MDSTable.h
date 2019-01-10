#ifndef MDSTABLE_H_INCLUDED
#define MDSTABLE_H_INCLUDED

namespace mds
{
class MDSTable
{
public:

    MDSTable();
    kvs::TableObject exec(const kvs::TableObject& input);
    kvs::TableObject exec(const double input[], size_t n);

private:

    size_t m_nrows; // number of input rows

    kvs::ValueArray<kvs::Real32> GetRowArray(const kvs::TableObject* table, const size_t i);
    kvs::Real32 GetEuclideanDistance(const kvs::ValueArray<kvs::Real32>& x0, const kvs::ValueArray<kvs::Real32>& x1);
    void DSMatrix(const kvs::TableObject& input, double output[]);
    void IPMatrix(const double input[], double output[]);
    kvs::TableObject OutputTable(double input[]);
};
}

#endif // MDSTABLE_H_INCLUDED
