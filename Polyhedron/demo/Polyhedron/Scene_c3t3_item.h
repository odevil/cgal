#ifndef SCENE_C3T3_ITEM_H
#define SCENE_C3T3_ITEM_H

#include "Scene_c3t3_item_config.h"
#include "C3t3_type.h"

#include <QVector>
#include <QColor>
#include <QPixmap>
#include <QMenu>
#include <set>

#include <QtCore/qglobal.h>
#include <CGAL/gl.h>
#include <QGLViewer/manipulatedFrame.h>
#include <QGLViewer/qglviewer.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include <CGAL/Three/Viewer_interface.h>

#include <CGAL/Three/Scene_item.h>
#include <Scene_polyhedron_item.h>
#include <Scene_polygon_soup_item.h>

struct Scene_c3t3_item_priv;

using namespace CGAL::Three;
  class SCENE_C3T3_ITEM_EXPORT Scene_c3t3_item
  : public Scene_item
{
  Q_OBJECT
public:
  typedef qglviewer::ManipulatedFrame ManipulatedFrame;

  Scene_c3t3_item();
  Scene_c3t3_item(const C3t3& c3t3);
  ~Scene_c3t3_item();

  void invalidate_buffers()
  {
    are_buffers_filled = false;
    compute_bbox();
  }

  void c3t3_changed();

  void contextual_changed()
  {
    if (frame->isManipulated() || frame->isSpinning())
      invalidate_buffers();
  }
  const C3t3& c3t3() const;
  C3t3& c3t3();

  bool manipulatable() const {
    return true;
  }
  ManipulatedFrame* manipulatedFrame() {
    return frame;
  }

  void setPosition(float x, float y, float z) {
    frame->setPosition(x, y, z);
  }

  void setNormal(float x, float y, float z) {
    frame->setOrientation(x, y, z, 0.f);
  }

  Kernel::Plane_3 plane() const;

  bool isFinite() const { return true; }
  bool isEmpty() const {
    return c3t3().triangulation().number_of_vertices() == 0;
  }

  void compute_bbox() const;

  Scene_c3t3_item* clone() const {
    return 0;
  }

  // data item
  const Scene_item* data_item() const;
  void set_data_item(const Scene_item* data_item);

  QString toolTip() const;

  // Indicate if rendering mode is supported
  bool supportsRenderingMode(RenderingMode m) const {
    return (m != Gouraud && m != PointsPlusNormals && m != Splatting);
  }

  void draw(CGAL::Three::Viewer_interface* viewer) const;
  void draw_edges(CGAL::Three::Viewer_interface* viewer) const;
  void draw_points(CGAL::Three::Viewer_interface * viewer) const;
private:
  void draw_triangle(const Kernel::Point_3& pa,
    const Kernel::Point_3& pb,
    const Kernel::Point_3& pc, bool /* is_cut */) const;

  void draw_triangle_edges(const Kernel::Point_3& pa,
    const Kernel::Point_3& pb,
    const Kernel::Point_3& pc)const;

  double complex_diag() const;

  void compute_color_map(const QColor& c);

  public Q_SLOTS:
  void export_facets_in_complex();

  void data_item_destroyed();

  void show_spheres(bool b)
  {
    spheres_are_shown = b;
    Q_EMIT redraw();

  }
  virtual QPixmap graphicalToolTip() const;

  void update_histogram();

  void changed();

private:
  void build_histogram();

  QColor get_histogram_color(const double v) const;

public:
  QMenu* contextMenu();

  void set_scene(CGAL::Three::Scene_interface* scene){ last_known_scene = scene; }

protected:
  Scene_c3t3_item_priv* d;

private:
  enum Buffer
  {
      Facet_vertices =0,
      Facet_normals,
      Edges_vertices,
      Grid_vertices,
      Sphere_vertices,
      Sphere_normals,
      Sphere_colors,
      Sphere_radius,
      Sphere_center,
      Wired_spheres_vertices,
      NumberOfBuffers
  };
  enum Vao
  {
      Facets=0,
      Edges,
      Grid,
      Spheres,
      Wired_spheres,
      NumberOfVaos
  };
  qglviewer::ManipulatedFrame* frame;
  CGAL::Three::Scene_interface* last_known_scene;

  bool spheres_are_shown;
  const Scene_item* data_item_;
  QPixmap histogram_;

  typedef std::set<int> Indices;
  Indices indices_;

  //!Allows OpenGL 2.1 context to get access to glDrawArraysInstanced.
  typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDARBPROC) (GLenum mode, GLint first, GLsizei count, GLsizei primcount);
  //!Allows OpenGL 2.1 context to get access to glVertexAttribDivisor.
  typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORARBPROC) (GLuint index, GLuint divisor);
  //!Allows OpenGL 2.1 context to get access to gkFrameBufferTexture2D.
  PFNGLDRAWARRAYSINSTANCEDARBPROC glDrawArraysInstanced;
  //!Allows OpenGL 2.1 context to get access to glVertexAttribDivisor.
  PFNGLVERTEXATTRIBDIVISORARBPROC glVertexAttribDivisor;

  mutable std::vector<float> positions_lines;
  mutable std::vector<float> positions_grid;
  mutable std::vector<float> positions_poly;
  mutable std::vector<float> normals;
  mutable std::vector<float> s_normals;
  mutable std::vector<float> s_colors;
  mutable std::vector<float> s_vertex;
  mutable std::vector<float> ws_vertex;
  mutable std::vector<float> s_radius;
  mutable std::vector<float> s_center;
  mutable QOpenGLShaderProgram *program;
  mutable QOpenGLShaderProgram *program_sphere;

  using Scene_item::initialize_buffers;
  void initialize_buffers(CGAL::Three::Viewer_interface *viewer)const;
  void compute_elements() const;
  void compile_shaders();
};

#endif // SCENE_C3T3_ITEM_H