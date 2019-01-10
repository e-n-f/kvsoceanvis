#ifndef BUTTONCONFIG_H_INCLUDED
#define BUTTONCONFIG_H_INCLUDED

class PosValue
{
private:

    static kvs::Vector2f m_real_minrange;
    static kvs::Vector2f m_real_maxrange;
    static kvs::Vector2f m_real_minpos;
    static kvs::Vector2f m_real_maxpos;
    static kvs::Vector2f m_obj_minrange;
    static kvs::Vector2f m_obj_maxrange;
    static kvs::Vector2f m_obj_minpos;
    static kvs::Vector2f m_obj_maxpos;
    static kvs::Vector2i m_win_minrange;
    static kvs::Vector2i m_win_maxrange;
    static kvs::Vector2i m_win_minpos;
    static kvs::Vector2i m_win_maxpos;
    static bool m_paintflag;

public:

    static void setRealRange(const float xmin, const float ymin, const float xmax, const float ymax);
    static void setRealPos(const float xmin, const float ymin, const float xmax, const float ymax);
    static void setObjRange(const float xmin, const float ymin, const float xmax, const float ymax);
    static void setObjPos(const float xmin, const float ymin, const float xmax, const float ymax);
    static void setWinRange(const int xmin, const int ymin, const int xmax, const int ymax);
    static void setWinPos(const int xmin, const int ymin, const int xmax, const int ymax);
    static void setPaintFlag(const bool paintflag);
    static void getRealRange(float* xmin, float* ymin, float* xmax, float* ymax);
    static void getRealPos(float* xmin, float* ymin, float* xmax, float* ymax);
    static void getObjRange(float* xmin, float* ymin, float* xmax, float* ymax);
    static void getObjPos(float* xmin, float* ymin, float* xmax, float* ymax);
    static void getWinRange(int* xmin, int* ymin, int* xmax, int* ymax);
    static void getWinPos(int* xmin, int* ymin, int* xmax, int* ymax);
    static bool getPaintFlag();
    static std::vector<int> temp;
    static void sortValue();
};

class UpdatePR : public kvs::KeyPressEventListener
{
private:

    kvs::glut::Screen* m_screen;
    kvs::glut::Screen* m_screen2;
    std::string m_object_name;
    std::string m_object_name2;
    std::string m_outputfile;
    kvs::TableObject* m_reftable;
    kvs::Vector2f m_pos_v0;
    kvs::Vector2f m_pos_v1;
    kvs::Vector3i m_posidx;
    kvs::Vector3i m_vindices;
    bool m_hidepos;
    bool m_hideuvw;

public:

    UpdatePR( kvs::glut::Screen* screen, kvs::glut::Screen* screen2, std::string object_name, std::string object_name2, std::string outputfile, kvs::TableObject* reftable, kvs::Vector3i posidx, kvs::Vector3i vindices, bool hidepos, bool hideuvw );
    void setPosValue(const float xmin, const float ymin, const float xmax, const float ymax);
    void update( kvs::KeyEvent* event );
};

void MouseProcess(const int x, const int y, const int action, const kvs::Vector2f objmin, const kvs::Vector2f objmax, kvs::glut::Screen* screen, const std::string object_name, UpdatePR* pr_updator);

kvs::Vector3d GetObjectPosition3( int x, int y, double depth, kvs::glut::Screen* screen, kvs::ObjectBase* object );

kvs::Vector2f GetObjectPosition2( int x, int y, kvs::glut::Screen* screen, const std::string& object_name );

void GetPosition(const int x0, const int y0, const int x1, const int y1, const kvs::Vector2f objmin, const kvs::Vector2f objmax, kvs::glut::Screen* screen, const std::string object_name, UpdatePR* pr_updator);

class MousePress : public kvs::MousePressEventListener
{
private:

    kvs::glut::Screen* m_screen;
    std::string m_object_name;
    kvs::Vector2f m_objmin;
    kvs::Vector2f m_objmax;
    UpdatePR* m_pr_updator;

public:

    MousePress(kvs::glut::Screen* screen, std::string object_name, kvs::Vector2f objmin, kvs::Vector2f objmax, UpdatePR* pr_updator);
    void update( kvs::MouseEvent* event );
};

class MouseRelease : public kvs::MouseReleaseEventListener
{
private:

    kvs::glut::Screen* m_screen;
    std::string m_object_name;
    kvs::Vector2f m_objmin;
    kvs::Vector2f m_objmax;
    UpdatePR* m_pr_updator;

public:

    MouseRelease(kvs::glut::Screen* screen, std::string object_name, kvs::Vector2f objmin, kvs::Vector2f objmax, UpdatePR* pr_updator);
    void update( kvs::MouseEvent* event );
};

class MouseMove : public kvs::MouseMoveEventListener
{
private:

    kvs::glut::Screen* m_screen;

public:

    MouseMove(kvs::glut::Screen* screen);
    void update( kvs::MouseEvent* event );
};

class WheelScroll : public kvs::WheelEventListener
{
private:

public:

    void update( kvs::WheelEvent* event );
};

class PaintEvent : public kvs::PaintEventListener
{
private:

public:

    void update();
};

class UpdatePCP : public kvs::KeyPressEventListener
{
private:

    kvs::glut::Screen* m_pcpscreen;
    kvs::glut::Screen* m_vrscreen;
    kvs::glut::Screen* m_spmscreen;
    std::string m_tabobject_name;
    std::string m_tabobject2_name;
    std::string m_pcprenderer_name;
    std::string m_volobject_name;
    std::string m_lineobject_name;
    kvs::Vector3i m_posidx;
    kvs::Vector3i m_vindices;
    bool m_inverse;
    bool m_hidepos;
    bool m_hideuvw;
    std::string m_tableout;
    std::string m_volumeout;
    std::string m_volumeout3;
    std::string m_ivreffile;
    kvs::TableObject* m_reftable;

public:

    UpdatePCP();
    void setScreen(kvs::glut::Screen* pcpscreen, kvs::glut::Screen* vrscreen, kvs::glut::Screen* spmscreen);
    void setObject(const std::string tabobject_name, const std::string volobject_name, const std::string lineobject_name, const std::string tabobject2_name, kvs::TableObject* reftable);
    void setRenderer(const std::string pcprenderer_name);
    void setIndex(const kvs::Vector3i posidx, const kvs::Vector3i vindices);
    void setBool(const bool inverse, const bool hidepos, const bool hideuvw);
    void setFile(const std::string tableout, const std::string volumeout, const std::string volumeout3, const std::string ivreffile);
    void update( kvs::KeyEvent* event );
};

#endif // BUTTONCONFIG_H_INCLUDED
