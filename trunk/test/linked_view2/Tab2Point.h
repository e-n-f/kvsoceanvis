#ifndef TAB2POINT_H_INCLUDED
#define TAB2POINT_H_INCLUDED

namespace ttp
{
class TPConv
{
public:

    TPConv();
    void exec(const kvs::TableObject* input, const int xidx, const int yidx, const int vidx, const int psize, const float zvalue, kvs::ColorMap cmap, const std::string filename);

private:

};
}
#endif // TAB2POINT_H_INCLUDED
