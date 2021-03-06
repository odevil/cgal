#include "config.h"
#include "Scene_points_with_normal_item.h"
#include <CGAL/Three/Polyhedron_demo_plugin_helper.h>
#include <CGAL/Three/Polyhedron_demo_plugin_interface.h>

#include <QObject>
#include <QAction>
#include <QMainWindow>
#include <QApplication>
#include <QtPlugin>
#include <QInputDialog>

#include <CGAL/jet_smooth_point_set.h>

// Concurrency
#ifdef CGAL_LINKED_WITH_TBB
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif

using namespace CGAL::Three;
class Polyhedron_demo_point_set_smoothing_plugin :
  public QObject,
  public Polyhedron_demo_plugin_helper
{
  Q_OBJECT
  Q_INTERFACES(CGAL::Three::Polyhedron_demo_plugin_interface)
  Q_PLUGIN_METADATA(IID "com.geometryfactory.PolyhedronDemo.PluginInterface/1.0")

  QAction* actionJetSmoothing;

public:
  void init(QMainWindow* mainWindow, CGAL::Three::Scene_interface* scene_interface) {
    actionJetSmoothing = new QAction(tr("Point Set Jet Smoothing"), mainWindow);
    actionJetSmoothing->setObjectName("actionJetSmoothing");

    Polyhedron_demo_plugin_helper::init(mainWindow, scene_interface);
  }

  QList<QAction*> actions() const {
    return QList<QAction*>() << actionJetSmoothing;
  }

  bool applicable(QAction*) const { 
    return qobject_cast<Scene_points_with_normal_item*>(scene->item(scene->mainSelectionIndex()));
  }

public Q_SLOTS:
  void on_actionJetSmoothing_triggered();

}; // end Polyhedron_demo_point_set_smoothing_plugin

void Polyhedron_demo_point_set_smoothing_plugin::on_actionJetSmoothing_triggered()
{
  const CGAL::Three::Scene_interface::Item_id index = scene->mainSelectionIndex();

  Scene_points_with_normal_item* item =
    qobject_cast<Scene_points_with_normal_item*>(scene->item(index));

  if(item)
  {
    Point_set* points = item->point_set();
    if(!points) return;

    // Gets options
    bool ok;

    const unsigned int nb_neighbors =
      QInputDialog::getInt((QWidget*)mw,
                           tr("Jet Smoothing"), // dialog title
                           tr("Number of neighbors:"), // field label
                           24, // default value = fast
                           6, // min
                           1000, // max
                           1, // step
                           &ok);
    if(!ok) return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    CGAL::jet_smooth_point_set<Concurrency_tag>(points->begin(), points->end(), nb_neighbors);

    points->invalidate_bounds();

    // calling jet_smooth_point_set breaks the normals
    item->set_has_normals(false);

    // update scene
    item->invalidateOpenGLBuffers();
    scene->itemChanged(index);

    QApplication::restoreOverrideCursor();
  }
}

#include "Point_set_smoothing_plugin.moc"
