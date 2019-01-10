#include "Streamline.h"
#include <kvs/Type>
#include <kvs/Message>
#include <kvs/RGBColor>
#include <kvs/Vector3>
#include <kvs/VolumeObjectBase>


namespace
{

const int zpad_low = 0;
const int zpad_high = 30;
const int ypad_low = 195;
const int ypad_high = 310;
const int xpad_low = 230;
const int xpad_high = 320;


template <typename T>
inline const kvs::Vector3f GetInterpolatedVector(const size_t vertex_id[8], const float weight[8], const kvs::VolumeObjectBase* volume)
{
    const T* values = static_cast<const T*>(volume->values().data());

    kvs::Vector3f ret = kvs::Vector3f::Zero();
    for (size_t i = 0; i < 8; i++)
    {
        const size_t index = 3 * vertex_id[i];
        const float w = weight[i];
        ret.x() += static_cast<float>(values[index + 0] * w);
        ret.y() += static_cast<float>(values[index + 1] * w);
        ret.z() += static_cast<float>(values[index + 2] * w);
    }

    return ret;
}

template <typename T>
void PushBack(std::vector<T>& dst, const kvs::Vector3<T>& val)
{
    dst.push_back(val[0]);
    dst.push_back(val[1]);
    dst.push_back(val[2]);
}

void PushBack(std::vector<kvs::UInt8>& dst, const kvs::RGBColor& val)
{
    dst.push_back(val.r());
    dst.push_back(val.g());
    dst.push_back(val.b());
}

} // end of namespace


namespace test
{

Streamline::Streamline():
    kvs::MapperBase(),
    m_integration_method(Streamline::RungeKutta2nd),
    m_integration_interval(0.35f),
    m_vector_length_threshold(0.000001f),
    m_integration_times_threshold(256),
    m_enable_vector_length_condition(true),
    m_enable_integration_times_condition(true),
    m_vortexes(NULL)
{
}

Streamline::~Streamline()
{
    delete m_vortexes;
}

void Streamline::setVortexes(const kvs::PointObject* vortexes)
{
    m_vortexes = new kvs::PointObject();
    m_vortexes->setCoords( vortexes->coords() );
    m_vortexes->setColor( kvs::RGBColor( 255, 255, 255 ) );
    m_vortexes->setSize( 1 );
}

void Streamline::setIntegrationMethod(const Streamline::IntegrationMethod method)
{
    m_integration_method = method;
}

void Streamline::setIntegrationInterval(const float interval)
{
    m_integration_interval = interval;
}

void Streamline::setVectorLengthThreshold(const float length)
{
    m_vector_length_threshold = length;
}

void Streamline::setIntegrationTimesThreshold(const size_t times)
{
    m_integration_times_threshold = times;
}

void Streamline::setEnableVectorLengthCondition(const bool enabled)
{
    m_enable_vector_length_condition = enabled;
}

void Streamline::setEnableIntegrationTimesCondition(const bool enabled)
{
    m_enable_integration_times_condition = enabled;
}

kvs::LineObject* Streamline::exec(const kvs::ObjectBase* object)
{
    BaseClass::setSuccess( false );
    if (!object) throw std::logic_error("Input object is NULL.");

    const kvs::VolumeObjectBase* volume = kvs::VolumeObjectBase::DownCast(object);
    if (!volume) throw std::logic_error("Input object is not volume dat.");

    // Check whether the volume can be processed or not.
    if (volume->veclen() != 3) throw std::logic_error("Input volume is not vector field data.");

    // Attach the pointer to the volume object.
    BaseClass::attachVolume(volume);
    BaseClass::setRange(volume);
    BaseClass::setMinMaxCoords(volume, this);

    // set the min/max vector length.
    if (!volume->hasMinMaxValues())
        volume->updateMinMaxValues();

    this->mapping(volume);
    BaseClass::setSuccess( true );
    return this;
}

void Streamline::mapping(const kvs::VolumeObjectBase* volume)
{
    if (volume->volumeType() == kvs::VolumeObjectBase::Structured)
    {
        this->extract_lines(static_cast<const kvs::StructuredVolumeObject&>(*volume));
    }
    else // volume->volumeType() == kvs::VolumeObjectBase::Unstructured
    {
        throw std::logic_error("Unstructured volume object is not supported in the current system.");
    }
}

void Streamline::extract_lines(const kvs::StructuredVolumeObject& volume)
{
    m_cell.attach(volume);
    m_cell.setIgnoreValue(m_ignore_value);

    // Calculated data arrays.
    std::vector<kvs::Real32> coords;
    std::vector<kvs::UInt32> connections;
    std::vector<kvs::UInt8>  colors;

    // Calculate streamline for each seed point.
    const size_t npoints = m_vortexes->numberOfVertices();
    m_vortex_radius.clear();

    for (size_t index = 0; index < npoints; ++index)
    {
        const kvs::Vector3f vortex = m_vortexes->coord(index);
        double eigen_value;
        kvs::Vector3d eigen_vector;
        eigen_value = m_cell.realEigenValue(vortex, eigen_vector);
        kvs::Vector3d vec1;
        kvs::Vector3d vec2;
        kvs::Vector3d vec3 = eigen_vector;
        if (eigen_vector.x() == 0 && eigen_vector.y() == 0)
        {
            vec1 = kvs::Vector3d(1, 0, 0);
            vec2 = kvs::Vector3d(0, 1, 0);
        }
        else
        {
            vec1 = eigen_vector.cross(kvs::Vector3d(0,0,1));
            vec2 = vec1.cross(eigen_vector);
        }
        vec1.normalize();
        vec2.normalize();
        vec3.normalize();
        kvs::Matrix33f tmat;
        tmat[0] = kvs::Vector3f((float)vec1.x(), (float)vec2.x(), (float)vec3.x());
        tmat[1] = kvs::Vector3f((float)vec1.y(), (float)vec2.y(), (float)vec3.y());
        tmat[2] = kvs::Vector3f((float)vec1.z(), (float)vec2.z(), (float)vec3.z());

        kvs::Vector3f output_seeds[4];
        {
            float c = 0.1;
            output_seeds[0] = kvs::Vector3f((float)vec1.x(), (float)vec1.y(), (float)vec1.z()) * c;
            output_seeds[1] = - kvs::Vector3f((float)vec1.x(), (float)vec1.y(), (float)vec1.z()) * c;
            output_seeds[2] = kvs::Vector3f((float)vec2.x(), (float)vec2.y(), (float)vec2.z()) * c;
            output_seeds[3] = - kvs::Vector3f((float)vec2.x(), (float)vec2.y(), (float)vec2.z()) * c;
        }
        float vortex_radius = 0;
        for (int i = 0; i < 4; ++i)
        {
            std::vector<kvs::Real32> line_coords;
            std::vector<kvs::UInt8> line_colors;

            kvs::Vector3f seed = output_seeds[i] + vortex;

            m_direction_factor = 1;
            float forward_rad = this->calculate_radius(seed, vortex, tmat);
            m_direction_factor = -1;
            float backward_rad = this->calculate_radius(seed, vortex, tmat);

            vortex_radius = kvs::Math::Max(vortex_radius, forward_rad, backward_rad);

            this->calculate_line(seed, line_coords, line_colors);

            // Set the first vertex ID to the connections.
            const size_t dimension = 3;
            const size_t start_id = coords.size() / dimension;
            connections.push_back(static_cast<kvs::UInt32>(start_id));

            // Set the line coordinate and color value array to the coords and colors, respectively.
            const size_t ncoords = line_coords.size();
            for(size_t i = 0; i < ncoords; ++i)
            {
                coords.push_back(line_coords[i]);
                colors.push_back(line_colors[i]);
            }

            // Set the last vertex ID to the connections.
            const size_t last_id = coords.size() / dimension - 1;
            connections.push_back(static_cast<kvs::UInt32>(last_id));
        }
        m_vortex_radius.push_back(vortex_radius);
    }

    SuperClass::setLineType(kvs::LineObject::Polyline);
    SuperClass::setColorType(kvs::LineObject::VertexColor);
    SuperClass::setCoords(kvs::ValueArray<kvs::Real32>(coords));
    SuperClass::setConnections(kvs::ValueArray<kvs::UInt32>(connections));
    SuperClass::setColors(kvs::ValueArray<kvs::UInt8>(colors));
    SuperClass::setSize(1.0f);
}

bool crossWithAxis(const kvs::Vector3f& a, const kvs::Vector3f& b, float& intercept)
{
    if (a.y() * b.y() < 0)
    {
        intercept = - a.x() + (a.x() - b.x()) / (a.y() - b.y()) * a.y();
        return intercept > 0;
    }
    return false;
}


float Streamline::calculate_radius(
    const kvs::Vector3f& seed_point,
    const kvs::Vector3f& vortex,
    const kvs::Matrix33f& tmat)
{
    if (this->is_inside(seed_point))
    {
        float radius = 0;
        kvs::Vector3f current_vertex = seed_point;
        kvs::Vector3f next_vertex = seed_point;

        float distance = 0;

        const int max_repeat = 10000;
        const float threshold = 1.0f;
        for (int i = 0; i < max_repeat; ++i)
        {
            // Calculate the next vertex.
            if (!this->calculate_next_vertex(next_vertex)) break;
            // Check the termination.
            if (!this->is_inside(next_vertex)) break;

            kvs::Vector3f prev_local = tmat * (current_vertex - vortex);
            kvs::Vector3f next_local = tmat * (next_vertex - vortex);
            float temp = 0;
            if (crossWithAxis(prev_local, next_local, temp))
            {
                float abs_distance = kvs::Math::Abs(temp - distance);
                distance = temp;

                if (abs_distance < threshold)
                {
                    radius = kvs::Math::Max<float>(radius, distance);
                }
                else break;
            }
            current_vertex = next_vertex;
        }
        return radius;
    }
    return 0;
}

void Streamline::calculate_line(
    const kvs::Vector3f& seed_point,
    std::vector<kvs::Real32>& coords,
    std::vector<kvs::UInt8>& colors)
{
    KVS_ASSERT(coords.size() == 0 && colors.size() == 0);

    if (!this->is_inside(seed_point)) return;

    const kvs::Vector3f seed_vector = this->get_vector_value(seed_point);
    //if (m_integration_direction == Streamline::ForwardDirection)
    //{
    //    // Forward direction.
    //    m_direction_factor = 1;
    //    this->calculate_one_side(seed_point, coords, colors);
    //}
    //else if (m_integration_direction == Streamline::BackwardDirection)
    //{
    //    // Backward direction.
    //    m_direction_factor = -1;
    //    this->calculate_one_side(seed_point, coords, colors);
    //}
    //else // m_direction == Streamline::BothDirections
    {
        // Forward direction.
        m_direction_factor = 1;
        this->calculate_one_side(seed_point, coords, colors);

        // Reverses the line
        const size_t n = coords.size() / 3;
        const size_t halfn = n / 2;
        for (size_t i = 0; i < halfn; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                std::swap(coords[i * 3 + j], coords[(n - 1 - i) * 3 + j]);
                std::swap(colors[i * 3 + j], colors[(n - 1 - i) * 3 + j]);
            }
        }

        // Removes the seed point.
        for (size_t i = 0; i < 3; ++i)
        {
            coords.pop_back();
            colors.pop_back();
        }

        // Backward direction.
        m_direction_factor = -1;
        this->calculate_one_side(seed_point, coords, colors);
    }
}

void Streamline::calculate_one_side(
    const kvs::Vector3f& seed_point,
    std::vector<kvs::Real32>& coords,
    std::vector<kvs::UInt8>& colors)
{
    if (!this->is_inside(seed_point)) return;

    kvs::Vector3f current_vertex = seed_point;
    kvs::Vector3f next_vertex;

    size_t integral_times = 0;
    for (;;)
    {
        PushBack(coords, current_vertex);
        PushBack(colors, this->calculate_color(current_vertex));

        // Calculate the next vertex.
        if (!this->calculate_next_vertex(current_vertex)) return;

        // Check the termination.
        if (!this->is_inside(current_vertex)) return;
        if (m_enable_vector_length_condition && this->get_vector_value(current_vertex).length() <= m_vector_length_threshold) return;
        if (m_enable_integration_times_condition && integral_times >= m_integration_times_threshold) return;

        ++integral_times;
    }
}

bool Streamline::calculate_next_vertex(kvs::Vector3f& position)
{
    switch (m_integration_method)
    {
    case Streamline::Euler:
        return this->integrate_by_euler(position);
    case Streamline::RungeKutta2nd:
        return this->integrate_by_runge_kutta_2nd(position);
    case Streamline::RungeKutta4th:
        return this->integrate_by_runge_kutta_4th(position);
    default: break;
    }

    throw std::logic_error("Specified integral method is not defined.");
}

bool Streamline::integrate_by_euler(kvs::Vector3f& position)
{
    if (!this->is_inside(position)) return false;

    const kvs::Vector3f k1 = m_integration_interval * this->get_direction(position);
    position += k1;
    return true;
}

bool Streamline::integrate_by_runge_kutta_2nd(kvs::Vector3f& position)
{
    if (!this->is_inside(position)) return false;

    const kvs::Vector3f k1 = this->get_direction(position);
    position += 0.5f * m_integration_interval * k1;

    if (!this->is_inside(position)) return false;

    const kvs::Vector3f k2 = this->get_direction(position);
    position += m_integration_interval * k2;

    return true;
}

bool Streamline::integrate_by_runge_kutta_4th(kvs::Vector3f& position)
{
    if (!this->is_inside(position)) return false;
    const kvs::Vector3f k1 = this->get_direction(position);

    kvs::Vector3f temp;

    temp = position + 0.5f * m_integration_interval * k1;
    if (!this->is_inside(temp)) return false;
    const kvs::Vector3f k2 = this->get_direction(temp);

    temp = position + 0.5f * m_integration_interval * k2;
    if (!this->is_inside(temp)) return false;
    const kvs::Vector3f k3 = this->get_direction(temp);

    temp = position + m_integration_interval * k3;
    if (!this->is_inside(temp)) return false;
    const kvs::Vector3f k4 = this->get_direction(temp);

    position += m_integration_interval / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
    return true;
}

bool Streamline::is_inside(const kvs::Vector3f& point)
{
    const kvs::StructuredVolumeObject& structured_volume = *static_cast<const kvs::StructuredVolumeObject*>(BaseClass::volume());
    const float dimx = static_cast<float>(structured_volume.resolution().x() - 1);
    const float dimy = static_cast<float>(structured_volume.resolution().y() - 1);
    const float dimz = static_cast<float>(structured_volume.resolution().z() - 1);

    float x = point.x();
    float y = point.y();
    float z = point.z();

/*
    if (x < xpad_low || dimx - xpad_high < x
     || y < ypad_low || dimy - ypad_high < y
     || z < zpad_low || dimz - zpad_high < z)
        return false;
*/
    if (x < 0 || dimx < x
     || y < 0 || dimy < y
     || z < 0 || dimz < z)
        return false;

    m_cell.loadFromPoint(point);
    if (m_cell.includesIgnoreValue())
        return false;
    return true;
}

kvs::RGBColor Streamline::calculate_color(const kvs::Vector3f& position) const
{
    kvs::Vector3f vector_value = this->get_vector_value(position);

    const kvs::Real64 min_value = BaseClass::volume()->minValue();
    const kvs::Real64 max_value = BaseClass::volume()->maxValue();
    const kvs::UInt8 level = kvs::UInt8(255.0 * (vector_value.length() - min_value) / (max_value - min_value));

    return BaseClass::transferFunction().colorMap()[level];
}

kvs::Vector3f Streamline::get_direction(const kvs::Vector3f& position) const
{
    m_cell.loadFromPoint(position);
    kvs::Vector3f vec = m_cell.getValue(position - m_cell.globalBasePoint());
    return vec.normalized() * m_direction_factor;
}

kvs::Vector3f Streamline::get_vector_value(const kvs::Vector3f& position) const
{
    const size_t cell_x = static_cast<size_t>(position.x());
    const size_t cell_y = static_cast<size_t>(position.y());
    const size_t cell_z = static_cast<size_t>(position.z());

    const kvs::StructuredVolumeObject* volume = kvs::StructuredVolumeObject::DownCast(BaseClass::volume());
    const size_t resolution_x = static_cast<size_t>(volume->resolution().x());
    const size_t resolution_y = static_cast<size_t>(volume->resolution().y());
//    const size_t resolution_z = static_cast<size_t>(volume->resolution().z());

    size_t vertex_id[8];
    vertex_id[0] = cell_z * resolution_x * resolution_y + cell_y * resolution_x + cell_x;
    vertex_id[1] = vertex_id[0] + 1;
    vertex_id[2] = vertex_id[1] + resolution_x;
    vertex_id[3] = vertex_id[2] - 1;
    vertex_id[4] = vertex_id[0] + resolution_x * resolution_y;
    vertex_id[5] = vertex_id[4] + 1;
    vertex_id[6] = vertex_id[5] + resolution_x;
    vertex_id[7] = vertex_id[6] - 1;

    // Weight.
    const kvs::Vector3f local_coord(
        2.0f * (position.x() - cell_x) - 1.0f,
        2.0f * (position.y() - cell_y) - 1.0f,
        2.0f * (position.z() - cell_z) - 1.0f);

    const float x_min = local_coord.x() - 1.0f;
    const float x_max = local_coord.x() + 1.0f;
    const float y_min = local_coord.y() - 1.0f;
    const float y_max = local_coord.y() + 1.0f;
    const float z_min = local_coord.z() - 1.0f;
    const float z_max = local_coord.z() + 1.0f;

    const float weight[8] = {
        -x_min * y_min * z_min * 0.125f,
        x_max  * y_min * z_min * 0.125f,
        -x_max * y_max * z_min * 0.125f,
        x_min  * y_max * z_min * 0.125f,
        x_min  * y_min * z_max * 0.125f,
        -x_max * y_min * z_max * 0.125f,
        x_max  * y_max * z_max * 0.125f,
        -x_min * y_max * z_max * 0.125f };

    // Interpolate.
    const std::type_info& type = BaseClass::volume()->values().typeInfo()->type();
    if (type == typeid(kvs::Real32))
        return ::GetInterpolatedVector<kvs::Real32>(vertex_id, weight, BaseClass::volume());

    throw std::logic_error("not supported");
}

}
