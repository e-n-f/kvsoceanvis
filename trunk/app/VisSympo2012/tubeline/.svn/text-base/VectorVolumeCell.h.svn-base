#ifndef MY_VECTOR_CELL
#define MY_VECTOR_CELL

#include <algorithm>
#include <exception>
#include <stdexcept>

#define MY_ASSERT(expr) my_check(expr)

inline void my_check(bool expr)
{
    if (!expr) throw std::logic_error("error");
}

class VectorVolumeCell
{
public:
    VectorVolumeCell()
    {
        m_ref_volume = NULL;
        m_ignore_value = 0;
    }

    void attach(const kvs::StructuredVolumeObject& volume)
    {
        m_dimx = volume.resolution().x();
        m_dimy = volume.resolution().y();
        m_dimz = volume.resolution().z();
        m_values_begin = static_cast<const float*>( volume.values().data() );
        m_ref_volume = &volume;
    }

    void loadFromPoint(const kvs::Vector3f& pos)
    {
        size_t index_x = static_cast<size_t>(pos.x());
        size_t index_y = static_cast<size_t>(pos.y());
        size_t index_z = static_cast<size_t>(pos.z());
        this->load(index_x, index_y, index_z);
    }

    void load(size_t index_x, size_t index_y, size_t index_z)
    {
        const double xscale = 9000;
        const double yscale = 10800;
        const double zscale[] = {
            1, 1.75, 2.75, 4, 5.5, 
            7, 10, 12, 14, 16, 
            17, 18, 19, 20, 20, 
            21, 22, 23, 24, 27, 
            30, 30, 35, 40, 40, 
            50, 60, 65, 70, 75, 
            80, 90, 100, 100, 100,
            100, 100, 100, 100, 100,
            100, 100, 100, 100, 100,
            100, 100, 100, 100, 100,
            100, 100, 100, 100, 100,
            100, 100, 100, 100, 100,
            100, 100, 100, 100, 100,
            0
        };

        MY_ASSERT(index_x < m_dimx);
        MY_ASSERT(index_y < m_dimy);
        MY_ASSERT(index_z < m_dimz);

        m_global_base_point = kvs::Vector3f((float)index_x, (float)index_y, (float)index_z);

        /*  p2 -------p3
        *   |         |    p0: (x0, y0)
        *   |         |    p1: (x1, y1)
        *   |         |    p2: (x2, y2)
        *  p0 ------- p1   p3: (x3, y3)
        */

        size_t index[8];
        index[0] = index_x + m_dimx * index_y + m_dimx * m_dimy * index_z;
        index[1] = index[0] + 1;
        index[2] = index[0] + m_dimx;
        index[3] = index[2] + 1;

        index[4] = index[0] + m_dimx * m_dimy;
        index[5] = index[4] + 1;
        index[6] = index[4] + m_dimx;
        index[7] = index[6] + 1;

        const float* pvalues = m_values_begin;
        for (int i = 0; i < 8; ++i)
        {
            double u = pvalues[index[i] * 3 + 0];
            double v = pvalues[index[i] * 3 + 1];
            double w = pvalues[index[i] * 3 + 2];

            m_cell_values[i +  0] = u == m_ignore_value ? m_ignore_value : u / xscale;
            m_cell_values[i +  8] = v == m_ignore_value ? m_ignore_value : v / yscale;
            m_cell_values[i + 16] = w == m_ignore_value ? m_ignore_value : w / zscale[index_z + i / 4];
        }
    }

    void findCriticalPoint(std::vector<float>& output) const
    {
        MY_ASSERT(m_ref_volume != NULL);

        if (this->includesIgnoreValue()) return;

        kvs::Vector3d current_local(0.5, 0.5, 0.5);
        const int max_loop = 1000;

        for (int i = 0; i < max_loop; ++i)
        { 
            const kvs::Matrix33d mat_inv = this->calcDifferentialTensor(current_local).inverted();
            const kvs::Vector3d difference = - mat_inv * this->getValue(current_local);
            if (difference.length() < 0.001)
            {
                if (!this->is_included(current_local) || !this->is_vortex(current_local))
                    return;

                kvs::Vector3f vortex = m_global_base_point + conv(current_local);
                output.push_back(vortex.x());
                output.push_back(vortex.y());
                output.push_back(vortex.z());
                return;
            }
            current_local += difference;
        }
    }

    double realEigenValue(const kvs::Vector3f& vortex, kvs::Vector3d& eigen_vector)
    {
        this->loadFromPoint(vortex);
        kvs::Vector3f global_base_point = this->globalBasePoint();
        kvs::Vector3f vortex_local = vortex - global_base_point;

        kvs::Matrix33d mat = this->calcDifferentialTensor(conv(vortex_local));
        double a3 = 1;
        double a2 = - mat.trace();
        double a1 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]
                  + mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]
                  + mat[2][2] * mat[0][0] - mat[2][0] * mat[0][2];
        double a0 = - mat.determinant();

        double eigen_value = newton_method(a0, a1, a2, a3);

        eigen_vector = calc_eigen_vector(mat, eigen_value);

        return eigen_value;
    }

    kvs::Vector3d eigenVector(const kvs::Vector3f& vortex, double eigen_value)
    {
        this->loadFromPoint(vortex);
        kvs::Vector3f global_base_point = this->globalBasePoint();
        kvs::Vector3f vortex_local = vortex - global_base_point;

        kvs::Matrix33d mat = this->calcDifferentialTensor(this->conv(vortex_local));
        return calc_eigen_vector(mat, eigen_value);
    }

    kvs::Vector3f globalBasePoint() const
    {
        return m_global_base_point;
    }

    void setIgnoreValue(float value)
    {
        m_ignore_value = value;
    }

    bool includesIgnoreValue() const
    {
        return std::find(m_cell_values, m_cell_values + 24, m_ignore_value) != (m_cell_values + 24);
    }

    kvs::Vector3d getValue(const kvs::Vector3d& local) const
    {
        const double u = trilinear_interpolate( 0 + m_cell_values, local);
        const double v = trilinear_interpolate( 8 + m_cell_values, local);
        const double w = trilinear_interpolate(16 + m_cell_values, local);
        return kvs::Vector3d(u, v, w);
    }

    kvs::Vector3f getValue(const kvs::Vector3f local) const
    {
        return this->conv(this->getValue(this->conv(local)));
    }

private:
    kvs::Matrix33d calcDifferentialTensor(const kvs::Vector3d& local) const
    {
        kvs::Matrix33d ret;
        ret[0][0] = dx( 0 + m_cell_values, local); // dudx
        ret[0][1] = dy( 0 + m_cell_values, local); // dudy
        ret[0][2] = dz( 0 + m_cell_values, local); // dudz
        ret[1][0] = dx( 8 + m_cell_values, local); // dvdx
        ret[1][1] = dy( 8 + m_cell_values, local); // dvdy
        ret[1][2] = dz( 8 + m_cell_values, local); // dvdz
        ret[2][0] = dx(16 + m_cell_values, local); // dwdx
        ret[2][1] = dy(16 + m_cell_values, local); // dwdy
        ret[2][2] = dz(16 + m_cell_values, local); // dwdz
        return ret;
    }

    bool is_vortex(const kvs::Vector3d& local) const
    {
        kvs::Matrix33d mat = this->calcDifferentialTensor(local);
        double a3 = 1;
        double a2 = - mat.trace();
        double a1 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]
                  + mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]
                  + mat[2][2] * mat[0][0] - mat[2][0] * mat[0][2];
        double a0 = - mat.determinant();
        return has_imaginary_root(a0, a1, a2, a3);
    }

    static kvs::Vector3d calc_eigen_vector(const kvs::Matrix33d& mat, double eigen_value)
    {
        kvs::Matrix33d temp = mat;
        temp[0][0] -= eigen_value;
        temp[1][1] -= eigen_value;
        temp[2][2] -= eigen_value;

        if (temp[0][0] == 0) throw std::logic_error("souteigai desu");

        temp[0] /= temp[0][0];
        temp[1] -= temp[1][0] * temp[0];
        temp[2] -= temp[2][0] * temp[0];

        if (temp[1][1] == 0) throw std::logic_error("souteigai desu");

        temp[1] /= temp[1][1];
        temp[0] -= temp[0][1] * temp[1];
        temp[2] -= temp[2][1] * temp[1];

        kvs::Vector3d ret;
        ret.x() = -temp[0][2];
        ret.y() = -temp[1][2];
        ret.z() = 1;

        ret.normalize();
        return ret;
    }

    static kvs::Vector3d conv(const kvs::Vector3f& v)
    {
        return kvs::Vector3d(v.x(), v.y(), v.z());
    }

    static kvs::Vector3f conv(const kvs::Vector3d& v)
    {
        return kvs::Vector3f((float)v.x(), (float)v.y(), (float)v.z());
    }

    static double interpolate(double a, double b, double factor)
    {
        return (1 - factor) * a + factor * b;
    }

    static double bilinear_interpolate(double v0, double v1, double v2, double v3, double fact0, double fact1)
    {
        return interpolate(interpolate(v0, v1, fact0), interpolate(v2, v3, fact0), fact1);
    }

    static double trilinear_interpolate(const double* v, const kvs::Vector3d& p)
    {
        return  interpolate(
                    bilinear_interpolate(v[0], v[1], v[2], v[3], p.x(), p.y()),
                    bilinear_interpolate(v[4], v[5], v[6], v[7], p.x(), p.y()),
                    p.z());
    }

    static bool is_included(const kvs::Vector3d& p)
    {
        return p.x() >= 0 && p.x() < 1 && p.y() >= 0 && p.y() < 1 && p.z() >= 0 && p.z() < 1;
    }

    static double newton_method(double a0, double a1, double a2, double a3)
    {
        const double eps = 1.0e-4;
        const int max_times = 10000;
        double x = 1000;
        for (int count = 0; count < max_times; ++count)
        {
            double fx = ((a3 * x + a2) * x + a1) * x + a0;
            double dfx = (3 * a3 * x + 2 * a2) * x + a1;
            double diff = fx / dfx;
            if (kvs::Math::Abs(diff) < eps)
                break;
            x -= diff;
        }
        return x;
    }

    static double cube(double x)
    {
        return x * x * x;
    }

    static double sqr(double x)
    {
        return x * x;
    }

    static bool has_imaginary_root(double a0, double a1, double a2, double a3)
    {
        return - 4 * cube(a1) * a3
               + sqr(a1) * sqr(a2)
               - 4 * a0 * cube(a2)
               + 18 * a0 * a1 * a2 * a3
               - 27 * sqr(a0) * sqr(a3) < 0;
    }

    static double dx(const double* val, const kvs::Vector3d& p)
    {
        return bilinear_interpolate(val[1] - val[0], val[3] - val[2], val[5] - val[4], val[7] - val[6], p.y(), p.z());
    }

    static double dy(const double* val, const kvs::Vector3d& p)
    {
        return bilinear_interpolate(val[2] - val[0], val[6] - val[4], val[3] - val[1], val[7] - val[5], p.z(), p.x());
    }

    static double dz(const double* val, const kvs::Vector3d& p)
    {
        return bilinear_interpolate(val[4] - val[0], val[5] - val[1], val[6] - val[2], val[7] - val[3], p.x(), p.y());
    }

private:
    double m_cell_values[24];
    size_t m_dimx;
    size_t m_dimy;
    size_t m_dimz;
    const kvs::StructuredVolumeObject* m_ref_volume;
    const float* m_values_begin;
    kvs::Vector3f m_global_base_point;
    double m_ignore_value;
};

#endif
