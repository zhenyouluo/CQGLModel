#ifndef CGEOM_FILL_3D
#define CGEOM_FILL_3D

#include <CRGBA.h>
#include <CPoint2D.h>
#include <CVector3D.h>
#include <CMaterial.h>
#include <CImagePtr.h>
#include <vector>

class CGeomVertex3D;
class CGeomLight3D;
class CGeomZBuffer;
class CGeom3DRenderer;
class CGeomObject3D;

class CGeomFill3D {
 private:
  struct Point {
    bool      set;
    bool      stippled;
    bool      fogged;
    double    z;
    CRGBA     rgba;
    CVector3D normal;
    CPoint2D  tmap;

    Point() :
     set(false), stippled(false), fogged(false), z(0), rgba(0,0,0),
     normal(0,0,0), tmap(0,0) {
    }

    Point(double z1, const CRGBA &rgba1, const CVector3D &normal1,
          const CPoint2D &tmap1) :
     set(true), stippled(false), fogged(false), z(z1), rgba(rgba1),
     normal(normal1), tmap(tmap1) {
    }
  };

  struct Line {
    bool   set;
    uint   width;
    uint   start, end;
    Point *points;

    Line(uint width1=0) :
     set(false), width(width1), start(0), end(0), points(NULL) {
    }

    void clear() {
      set = false;

      for (uint x = start; x <= end; ++x)
        points[x].set = false;
    }
  };

  //------

  uint   width_;
  uint   height_;
  Line  *lines_;
  Point  def_point_;
  bool   z_buffer_;
  bool   flat_;
  bool   smooth_;
  bool   texture_;
  bool   writable_;

 public:
  class VertexAdapter {
   public:
    typedef std::vector<uint>            VertexIList;
    typedef std::vector<CGeomVertex3D *> VertexPList;

    virtual ~VertexAdapter() { }

    virtual uint getNumVertices() const = 0;

    virtual const CGeomVertex3D &getVertex(uint i) const = 0;
  };

  class VertexPAdapter : public VertexAdapter {
   private:
    const VertexPList &vertices_;

   public:
    VertexPAdapter(const VertexPList &vertices) :
     vertices_(vertices) {
    }

    uint getNumVertices() const {
      return vertices_.size();
    }

    const CGeomVertex3D &getVertex(uint i) const {
      return *vertices_[i];
    }
  };

  class VertexIAdapter : public VertexAdapter {
   private:
    CGeomObject3D     *object_;
    const VertexIList &vertices_;

   public:
    VertexIAdapter(CGeomObject3D *object, const VertexIList &vertices) :
     object_(object), vertices_(vertices) {
    }

    uint getNumVertices() const {
      return vertices_.size();
    }

    const CGeomVertex3D &getVertex(uint i) const;

   private:
    VertexIAdapter(const VertexIAdapter &rhs);
    VertexIAdapter &operator=(const VertexIAdapter &rhs);
  };

 public:
  CGeomFill3D(uint width=100, uint height=100);
 ~CGeomFill3D();

  void resize(uint width, uint height);

  void clear();

  uint getWidth () const { return width_ ; }
  uint getHeight() const { return height_; }

  void setDefaultColor(const CRGBA &rgba) {
    def_point_.rgba = rgba;
  }

  const CRGBA &getDefaultColor() const {
    return def_point_.rgba;
  }

  void setDefaultNormal(const CVector3D &normal) {
    def_point_.normal = normal;
  }

  const CVector3D &getDefaultNormal() const {
    return def_point_.normal;
  }

  void setZBuffer(bool z_buffer) {
    z_buffer_ = z_buffer;
  }

  bool getZBuffer() const {
    return z_buffer_;
  }

  void setFlat(bool flat) {
    flat_ = flat;
  }

  bool getFlat() const {
    return flat_;
  }

  void setSmooth(bool smooth) {
    smooth_ = smooth;
  }

  bool getSmooth() const {
    return smooth_;
  }

  void setTexture(bool texture) {
    texture_ = texture;
  }

  bool getTexture() const {
    return texture_;
  }

  void setWritable(bool writable) {
    writable_ = writable;
  }

  bool getWritable() const {
    return writable_;
  }

  void setPoint(uint x, uint y, const Point &point) {
    if (! z_buffer_) {
      Line *line = &lines_[y];

      if (! lines_[y].set) {
        line->set   = true;
        line->start = x;
        line->end   = x;
      }
      else {
        line->start = std::min(line->start, x);
        line->end   = std::max(line->end  , x);
      }

      Point *ppoint = &line->points[x];

      *ppoint = point;
    }
    else {
      if (point.z > 0) return;

      Line *line = &lines_[y];

      if (! lines_[y].set) {
        line->set   = true;
        line->start = x;
        line->end   = x;
      }
      else {
        line->start = std::min(line->start, x);
        line->end   = std::max(line->end  , x);
      }

      Point *ppoint = &line->points[x];

      if      (! ppoint->set)
        *ppoint = point;
      else if (point.z > ppoint->z) {
        if (writable_)
          *ppoint = point;
        else
          ppoint->rgba = CRGBA::modeCombine(point.rgba, ppoint->rgba,
                                            CRGBA_COMBINE_SRC_ALPHA,
                                            CRGBA_COMBINE_ONE_MINUS_SRC_ALPHA);
      }
    }
  }

  void fill(const VertexAdapter &vadapter);

  void stipple(CImagePtr stipple);

  void setStippled(bool flag);

  void light(const CMaterial &material, const CRGBA &ambient,
             const std::vector<CGeomLight3D *> &lights);

  void fogExp(double density);

  void setFogged(bool flag);

  void render(CGeom3DRenderer *renderer);

  void render(CGeomZBuffer *buffer);

 private:
  CGeomFill3D(const CGeomFill3D &rhs);
  CGeomFill3D &operator=(const CGeomFill3D &rhs);
};

#endif
