#ifndef KVS_TEST_STREAMLINE_BASE_H_INCLUDE
#define KVS_TEST_STREAMLINE_BASE_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/MapperBase>
#include <kvs/LineObject>
#include <kvs/PointObject>
#include <kvs/StructuredVolumeObject>
#include "VectorVolumeCell.h"

namespace kvs
{

namespace test
{
/*===========================================================================*/
/**
 *  Streamline class.
 */
/*===========================================================================*/
class Streamline : public kvs::MapperBase, public kvs::LineObject
{
    // Class name.
    kvsClassName(kvs::Streamline);

    // Module information.
    kvsModuleBaseClass(kvs::MapperBase);
    kvsModuleSuperClass(kvs::LineObject);

public:

    enum IntegrationMethod
    {
        Euler = 0,
        RungeKutta2nd = 1,
        RungeKutta4th = 2
    };

private:

    IntegrationMethod m_integration_method; ///< integtration method
    float m_integration_interval; ///< integration interval in the object coordinate
    float m_vector_length_threshold; ///< threshold of the vector length
    size_t m_integration_times_threshold; ///< threshold of the integration times
    bool m_enable_vector_length_condition; ///< flag for the vector length condition
    bool m_enable_integration_times_condition; ///< flag for the integration times
    float m_direction_factor;
    mutable VectorVolumeCell m_cell;
    float m_ignore_value;
    kvs::PointObject* m_vortexes; ///< seed points
    std::vector<float> m_vortex_radius;


public:

    Streamline();

    virtual ~Streamline();

public:

    void setVortexes(const kvs::PointObject* vortexes);

    void setIntegrationMethod(const Streamline::IntegrationMethod method);

    void setIntegrationInterval(const float interval);

    void setVectorLengthThreshold(const float length);

    void setIntegrationTimesThreshold(const size_t times);

    void setEnableVectorLengthCondition(const bool enabled);

    void setEnableIntegrationTimesCondition(const bool enabled);

    void setIgnoreValue(float ignore_value)
    {
        m_ignore_value = ignore_value;
    }

    const std::vector<float>& radiuses() const
    {
        return m_vortex_radius;
    }

public:

    kvs::LineObject* exec(const kvs::ObjectBase* object);

private:

    kvs::Vector3f get_vector_value(const kvs::Vector3f& position) const;

    kvs::RGBColor calculate_color(const kvs::Vector3f& position) const;

    kvs::Vector3f get_direction(const kvs::Vector3f& position) const;

private:

    void mapping(const kvs::VolumeObjectBase* volume);

    void extract_lines(const kvs::StructuredVolumeObject& volume);

    void calculate_line(const kvs::Vector3f& seed_point, std::vector<kvs::Real32>& vertices, std::vector<kvs::UInt8>& colors);

    void calculate_one_side(const kvs::Vector3f& seed_point, std::vector<kvs::Real32>& coords, std::vector<kvs::UInt8>& colors);

    float calculate_radius(
        const kvs::Vector3f& seed_point,
        const kvs::Vector3f& vortex,
        const kvs::Matrix33f& tmat);

    bool calculate_next_vertex(kvs::Vector3f& position);

    bool integrate_by_euler(kvs::Vector3f& position);

    bool integrate_by_runge_kutta_2nd(kvs::Vector3f& position);

    bool integrate_by_runge_kutta_4th(kvs::Vector3f& position);

private:

    bool is_inside(const kvs::Vector3f& position);

};

}

} // end of namespace kvs

#endif // KVS__STREAMLINE_BASE_H_INCLUDE
