#ifndef CGEOM_MASK_H
#define CGEOM_MASK_H

#include <CFile.h>
#include <CRGBA.h>
#include <CPoint2D.h>
#include <CImagePtr.h>
#include <CImage.h>
#include <CImageMgr.h>

class CGeomMaskImage {
 private:
  CImagePtr  image_;
  bool      *set_;

 public:
  CGeomMaskImage(CImagePtr image) :
   image_(image), set_(NULL) {
    uint w, h;

    getSize(&w, &h);

    set_ = new bool [w*h];

    CRGBA rgba;

    for (uint i = 0, y = 0; y < h; ++y) {
      for (uint x = 0; x < w; ++x, ++i) {
        image_->getRGBAPixel(x, y, rgba);

        set_[i] = (rgba.getGray() > 0.5);
      }
    }
  }

 ~CGeomMaskImage() {
    delete [] set_;
  }

  CImagePtr getImage() const { return image_; }

  void getSize(uint *width, uint *height) const {
    *width  = image_->getWidth ();
    *height = image_->getHeight();
  }

  bool getSet(uint x, uint y) const {
    uint w, h;

    getSize(&w, &h);

    x = std::min(std::max(x, 0U), w - 1);
    y = std::min(std::max(y, 0U), h - 1);

    uint ind = y*w + x;

    return set_[ind];
  }

 private:
  CGeomMaskImage(const CGeomMaskImage &rhs);
  CGeomMaskImage &operator=(const CGeomMaskImage &rhs);
};

class CGeomMaskMapping {
 public:
  typedef std::vector<CPoint2D> PointList;

 private:
  PointList points_;

 public:
  CGeomMaskMapping() :
   points_() {
  }

  CGeomMaskMapping(const PointList &points) :
   points_() {
    copy(points.begin(), points.end(), back_inserter(points_));
  }

  CGeomMaskMapping(CGeomMaskImage *mask_image, uint num_vertices=4) :
   points_() {
    uint width, height;

    mask_image->getSize(&width, &height);

    if      (num_vertices == 3) {
      points_.push_back(CPoint2D(         0   , 0         ));
      points_.push_back(CPoint2D( width - 1   , 0         ));
      points_.push_back(CPoint2D((width - 1)/2, height - 1));
    }
    else if (num_vertices == 4) {
      points_.push_back(CPoint2D(0        , 0         ));
      points_.push_back(CPoint2D(width - 1, 0         ));
      points_.push_back(CPoint2D(width - 1, height - 1));
      points_.push_back(CPoint2D(0        , height - 1));
    }
    else
      std::cerr << "CGeomMaskMapping: " << num_vertices << "vertices not supported" << std::endl;
  }

  CGeomMaskMapping(const CGeomMaskMapping &mapping) :
   points_(mapping.points_) {
  }

  uint numPoints() const { return points_.size(); }

  const CPoint2D &getPoint(uint i) const { return points_[i]; }
};

class CGeomMask {
 private:
  CGeomMaskImage   *image_;
  CGeomMaskMapping *mapping_;
  uint              num_points_;

 public:
  CGeomMask(CGeomMaskImage *image, CGeomMaskMapping *mapping=NULL) :
   image_(image), mapping_(mapping), num_points_(4) {
    if (mapping_ == NULL)
      setMapping(image_, num_points_);
  }

  CGeomMask(CImagePtr image, uint num_points=4) :
   image_(NULL), mapping_(NULL), num_points_(num_points) {
    image_ = new CGeomMaskImage(image);

    setMapping(image_, num_points);
  }

  CGeomMask(const std::string &filename, uint num_points=4) :
   image_(NULL), mapping_(NULL), num_points_(num_points) {
    CFile file(filename);

    CImageFileSrc src(file);

    CImagePtr image = CImageMgrInst->createImage(src);

    image->read(filename);

    if (image.isValid())
      image_ = new CGeomMaskImage(image);

    setMapping(image_, num_points);
  }

  CGeomMask(const CGeomMask &mask) :
   image_(mask.image_),
   mapping_(new CGeomMaskMapping(*mask.mapping_)),
   num_points_(mask.num_points_) {
  }

 ~CGeomMask() {
    delete image_;
    delete mapping_;
  }

  CGeomMask *dup() const {
    return new CGeomMask(*this);
  }

  void setImage(CGeomMaskImage *image) {
    delete image_;

    image_ = image;

    setMapping(image_, num_points_);
  }

  void setMapping(CGeomMaskMapping *mapping) {
    delete mapping_;

    mapping_ = mapping;
  }

  void setMapping(CGeomMaskImage *image, uint num_points=4) {
    delete mapping_;

    mapping_ = new CGeomMaskMapping(image, num_points);

    num_points_ = num_points;
  }

  void setMapping(const std::vector<CPoint2D> &points) {
    delete mapping_;

    mapping_ = new CGeomMaskMapping(points);

    num_points_ = points.size();
  }

  bool hasMapping() const { return (mapping_ != NULL); }

  CImagePtr getImage() const {
    return image_->getImage();
  }

  void getImageSize(uint *width, uint *height) {
    return image_->getSize(width, height);
  }

  uint numMappingPoints() const { return mapping_->numPoints(); }

  const CPoint2D &getMappingPoint(uint i) const {
    return mapping_->getPoint(i);
  }

  bool getImageSet(const CIPoint2D &point) {
    return image_->getSet(point.x, point.y);
  }

  bool getImageSet(uint x, uint y) const {
    return image_->getSet(x, y);
  }

  // disable assign/copy
 private:
  CGeomMask &operator=(const CGeomMask &mask);
};

#endif
