#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <assert.h>
#include <kvs/AnyValueArray>

#include "VortexQuant.h"

namespace vqt
{
    VQuant::VQuant()
    {
        // null
    }

    kvs::ValueArray<float> VQuant::exec(const kvs::ValueArray<float>& qvalues, const float r, const float c, const int xres, const int yres)
    {
        assert(qvalues.size()%(xres*yres) == 0);
        size_t n = qvalues.size();
        size_t ns = n / (xres*yres);
        kvs::ValueArray<float> output(n);
        std::vector<float> Qh;
        std::vector<float> Ah;
        for (size_t j=0; j<ns; j++)
        {
            kvs::ValueArray<float> qval(xres*yres);
            for (size_t i=0; i<xres*yres; i++)
            {
                qval[i] = qvalues[j*xres*yres+i];
            }
            float q = qval[0];
            for (size_t i=0; i<xres*yres; i++)
            {
                if (qval[i] < q)
                {
                    q = qval[i];
                }
            }
            if (q >= 0)
            {
                std::cout << "Warning: qmin = " << q << ", is not minus." << std::endl;
                for (size_t i=0; i<xres*yres; i++)
                {
                    output[j*xres*yres+i] = 0;
                }
            }
            else
            {
                float s = -q/r;
                float cq = 0;
                bool ifstop = false;
                while (ifstop != true)
                {
                    int a = 0;
                    for (size_t i=0; i<xres*yres; i++)
                    {
                        if (qval[i] <= q)
                        {
                            a += 1;
                        }
                    }
                    Ah.push_back(a);
                    Qh.push_back(q);
                    float rsq;
                    rsq = VQuant::cod(Qh, Ah);
                    if (rsq >= c)
                    {
                        cq = q;
                    }
                    q = q + s;
                    if (q > 0)
                    {
                        ifstop = true;
                    }
                }
                for (size_t i=0; i<xres*yres; i++)
                {
                    if (qval[i] <= cq)
                    {
                        output[j*xres*yres+i] = qval[i];
                    }
                    else
                    {
                        output[j*xres*yres+i] = 0;
                    }
                }
            }
        }
        return output;
    }

    float VQuant::cod(const std::vector<float>& q, const std::vector<float>& a)
    {
        assert(q.size() == a.size());
        size_t n = q.size();
        std::vector<float> c;
        std::vector<float> f(n);
        c = VQuant::LeastSquares(q, a);
        for (size_t i=0; i<n; i++)
        {
            f[i] = c[0]*q[i] + c[1];
        }
        float avg = 0;
        float sum = 0;
        for (size_t i=0; i<n; i++)
        {
            sum += a[i];
        }
        avg = sum / n;
        float rsq = 0;
        float tmp1 = 0;
        float tmp2 = 0;
        for (size_t i=0; i<n; i++)
        {
            tmp1 += (a[i] - f[i]) * (a[i] - f[i]);
        }
        for (size_t i=0; i<n; i++)
        {
            tmp2 += (a[i] - avg) * (a[i] - avg);
        }
        rsq = 1 - tmp1 / tmp2;
        return rsq;
    }

    std::vector<float> VQuant::LeastSquares(const std::vector<float>& x, const std::vector<float>& y)
    {
        assert(x.size() == y.size());
        std::vector<float> output(2);
        float a = 0;
        float b = 0;
        float c = 0;
        float d = float(x.size() + 1);
        float e = 0;
        for (size_t i=0; i<x.size(); i++)
        {
            a += x[i]*x[i];
            b += x[i];
            c += x[i]*y[i];
            e += y[i];
        }
        output[0] = (c*d - b*e) / (a*d - b*b);
        output[1] = (b*c - a*e) / (b*b - a*d);
        return output;
    }

    kvs::ValueArray<float> VQuant::QValue(const kvs::ValueArray<float>& coords, const kvs::ValueArray<float>& values, const int xres, const int yres, const int zres, const bool invert)
    {
        size_t n = coords.size()/3;
        kvs::ValueArray<float> output(n);
        std::vector<float> x(xres);
        std::vector<float> y(yres);
        std::vector<float> z(zres);
        std::vector<float> u(n);
        std::vector<float> v(n);
        for (size_t i=0; i<n; i++)
        {
            if (i>=0 && i<xres)
            {
                x[i] = coords[i*3];
            }
            if (i>=0 && i<yres)
            {
                y[i] = coords[i*3*xres+1];
            }
            if (i>=0 && i<zres)
            {
                z[i] = coords[i*3*xres*yres+2];
            }
            u[i] = values[i*2];
            v[i] = values[i*2+1];
        }
        std::vector<float> dudx;
        std::vector<float> dudy;
        std::vector<float> dvdx;
        std::vector<float> dvdy;
        float hx = x[1] - x[0];
        float hy = y[1] - y[0];
        dudx = VQuant::PartialDifferential(u, hx, xres, yres, true);
        dudy = VQuant::PartialDifferential(u, hy, xres, yres, false);
        dvdx = VQuant::PartialDifferential(v, hx, xres, yres, true);
        dvdy = VQuant::PartialDifferential(v, hy, xres, yres, false);
        if (invert == true)
        {
            for (size_t j=0; j<zres; j++)
            {
                for (size_t i=0; i<xres*yres; i++)
                {
                    size_t tidx = (zres - j - 1)*xres*yres + i;
                    size_t cidx = j*xres*yres + i;
                    output[tidx] = pow((dudx[cidx] - dvdy[cidx]), 2) + pow((dvdx[cidx] + dudy[cidx]), 2) - pow((dvdx[cidx] - dudy[cidx]), 2);
                    output[tidx] = output[tidx] / 2;
                }
            }
        }
        else
        {
            for (size_t i=0; i<n; i++)
            {
                output[i] = pow((dudx[i] - dvdy[i]), 2) + pow((dvdx[i] + dudy[i]), 2) - pow((dvdx[i] - dudy[i]), 2);
                output[i] = output[i] / 2;
            }
        }
        return output;
    }

    std::vector<float> VQuant::PartialDifferential(const std::vector<float>& input, const float h, const int xres, const int yres, const bool ifxaxis)
    {
        assert(input.size()%(xres*yres) == 0);
        std::vector<float> output(input.size());
        std::vector<float> ipts(xres*yres);
        for (size_t j=0; j<input.size()/(xres*yres); j++)
        {
            for (size_t i=0; i<xres*yres; i++)
            {
                ipts[i] = input[j*xres*yres+i];
            }
            if (ifxaxis == true)
            {
                for (size_t i=0; i<xres*yres; i++)
                {
                    if (i%xres == 0)
                    {
                        output[j*xres*yres+i] = VQuant::FiniteDifferenceMethod(ipts[i+1], ipts[i], h/2);
                    }
                    else if (i%xres == xres - 1)
                    {
                        output[j*xres*yres+i] = VQuant::FiniteDifferenceMethod(ipts[i], ipts[i-1], h/2);
                    }
                    else
                    {
                        output[j*xres*yres+i] = VQuant::FiniteDifferenceMethod(ipts[i+1], ipts[i-1], h);
                    }
                }
            }
            else
            {
                for (size_t i=0; i<xres*yres; i++)
                {
                    if (i%(xres*yres)>=0 && i%(xres*yres)<xres)
                    {
                        output[j*xres*yres+i] = VQuant::FiniteDifferenceMethod(ipts[i+xres], ipts[i], h/2);
                    }
                    else if (i%(xres*yres)>=xres*yres-xres && i%(xres*yres)<xres*yres)
                    {
                        output[j*xres*yres+i] = VQuant::FiniteDifferenceMethod(ipts[i], ipts[i-xres], h/2);
                    }
                    else
                    {
                        output[j*xres*yres+i] = VQuant::FiniteDifferenceMethod(ipts[i+xres], ipts[i-xres], h);
                    }
                }
            }
        }
        return output;
    }

    float VQuant::FiniteDifferenceMethod(const float fp, const float fm, const float h)
    {
        float output;
        output = (fp - fm) / (2 * h);
        return output;
    }
}
