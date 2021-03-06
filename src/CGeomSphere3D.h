#ifndef CGEOM_SPHERE_3D_H
#define CGEOM_SPHERE_3D_H

#include <CSphere3D.h>
#include <CGeomObject3D.h>

class CGeomSphere3D : public CGeomObject3D, public CSphere3D {
 private:
  CPoint3D center_;
  uint     num_xy_;
  uint     num_patches_;

 public:
  enum { NUM_XY      = 40 };
  enum { NUM_PATCHES = 40 };

  CGeomSphere3D(CGeomScene3D *pscene, const std::string &name,
                const CPoint3D &center, double radius);

 ~CGeomSphere3D() { }

  CGeomSphere3D *dup() const;

  static void addGeometry(CGeomObject3D *object, const CPoint3D &center,
                          double radius, uint num_xy=NUM_XY,
                          uint num_patches=NUM_PATCHES);

  void mapTexture(CGeomTexture *texture);
  void mapTexture(CImagePtr image);

  void mapMask(CGeomMask *mask);
  void mapMask(CImagePtr image);
};

#endif
