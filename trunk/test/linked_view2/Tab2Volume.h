#ifndef TAB2VOLUME_H_INCLUDED
#define TAB2VOLUME_H_INCLUDED

namespace ttv
{
class TVConv
{
public:

    TVConv();
    void exec(const kvs::TableObject* input, const kvs::Vector3i posidx, const int ndim, const int tidx, const kvs::Vector3i tidxes, const bool inverse, const std::string filename);

private:

    int calcres(const kvs::TableObject* input, const int index);
};
}
#endif // TAB2VOLUME_H_INCLUDED
