#ifndef VORTEXQUANT_H_INCLUDED
#define VORTEXQUANT_H_INCLUDED

namespace vqt
{
class VQuant
{
public:

    VQuant();
    kvs::ValueArray<float> exec(const kvs::ValueArray<float>& qvalues, const float r, const float c, const int xres, const int yres);
    kvs::ValueArray<float> QValue(const kvs::ValueArray<float>& coords, const kvs::ValueArray<float>& values, const int xres, const int yres, const int zres, const bool invert);

private:

    float cod(const std::vector<float>& q, const std::vector<float>& a);
    std::vector<float> LeastSquares(const std::vector<float>& x, const std::vector<float>& y);
    std::vector<float> PartialDifferential(const std::vector<float>& input, const float h, const int xres, const int yres, const bool ifxaxis);
    float FiniteDifferenceMethod(const float fp, const float fm, const float h);

};
}
#endif // VORTEXQUANT_H_INCLUDED
