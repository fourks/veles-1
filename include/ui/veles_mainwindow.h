/*
 * Copyright 2017 CodiLime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef VELES_MAINWINDOW_H
#define VELES_MAINWINDOW_H

#include <set>

#include <QDropEvent>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include <QStringList>
#include <QRubberBand>

#include "dbif/promise.h"
#include "dbif/types.h"

#include "ui/optionsdialog.h"

namespace veles {
namespace ui {

class MainWindowWithDetachableDockWidgets;

/*****************************************************************************/
/* DockWidget */
/*****************************************************************************/

class DockWidget : public QDockWidget {
  Q_OBJECT

 public:
  DockWidget();
  virtual ~DockWidget();
  bool markedAsActive();
  const QAction* maximizeHereAction();
  static DockWidget* getParentDockWidget(QObject* obj);

 public slots:
  void displayContextMenu(const QPoint& pos);
  void moveToDesktop();
  void moveToWindow();
  void detachToNewTopLevelWindow();
  void detachToNewTopLevelWindowAndMaximize();
  void topLevelChangedNotify(bool top_level);
  void switchTitleBar(bool is_default);
  void centerTitleBarOnPosition(QPoint pos);
  void splitHorizontally();
  void splitVertically();
  void setMarkedAsActive(bool active);

 protected:
  void moveEvent(QMoveEvent *event) Q_DECL_OVERRIDE;
  void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;
  QMenu* createMoveToDesktopMenu();
  QMenu* createMoveToWindowMenu();
  QAction* createMoveToNewWindowAction();
  QAction* createMoveToNewWindowAndMaximizeAction();
  void createSplitActions();

 protected:
  static constexpr int max_ticks_ = 4;
  static constexpr int step_msec_ = 100;

  int timer_id_;
  int ticks_;
  QMenu* context_menu_;
  QAction* detach_action_;
  QAction* maximize_here_action_;
  QAction* split_horizontally_action_;
  QAction* split_vertically_action_;
  QWidget* empty_title_bar_;
  bool marked_as_active_;
};

/*****************************************************************************/
/* TabBarEventFilter */
/*****************************************************************************/

class TabBarEventFilter : public QObject {
  Q_OBJECT

 public:
  TabBarEventFilter(QObject* parent = nullptr);

 public slots:
  void tabMoved(int from, int to);

 protected:
  bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
  virtual bool mouseMove(QTabBar* tab_bar, QMouseEvent* event);
  virtual bool mouseButtonPress(QTabBar* tab_bar, QMouseEvent* event);
  virtual bool mouseButtonRelease(QTabBar* tab_bar, QMouseEvent* event);
  virtual bool mouseButtonDblClick(QTabBar* tab_bar, QMouseEvent* event);

  QTabBar* dragged_tab_bar_;
  int dragged_tab_index_;
  QPoint drag_init_pos_;
  static const int k_drag_treshold_ = 5;
};

/*****************************************************************************/
/* MainWindowWithDetachableDockWidgets */
/*****************************************************************************/

class MainWindowWithDetachableDockWidgets: public QMainWindow {
  Q_OBJECT

 public:
  MainWindowWithDetachableDockWidgets(QWidget* parent = nullptr);
  virtual ~MainWindowWithDetachableDockWidgets();
  DockWidget* addTab(QWidget *widget, const QString &title,
      DockWidget* sibling = nullptr);
  void bringDockWidgetToFront(QDockWidget* dock_widget);
  void moveDockWidgetToWindow(DockWidget* dock_widget);
  void findTwoNonTabifiedDocks(DockWidget*& sibling1, DockWidget*& sibling2);
  DockWidget* findDockNotTabifiedWith(DockWidget* dock_widget);
  DockWidget* findDockNotTabifiedWith(QWidget* widget);
  QDockWidget* findSibling(QDockWidget* dock_widget);
  void setDockWidgetsWithNoTitleBars(bool no_title_bars);
  bool dockWidgetsWithNoTitleBars();
  QDockWidget* tabToDockWidget(QTabBar* tab_bar, int index);
  QTabBar* dockWidgetToTab(QDockWidget* dock_widget);
  void splitDockWidget2(QDockWidget* first, QDockWidget* second, Qt::Orientation orientation);
  void showRubberBand(bool show);

  static MainWindowWithDetachableDockWidgets* getParentMainWindow(
      QObject* obj);
  static bool intersectsWithAnyMainWindow(DockWidget* dock_widget);
  static MainWindowWithDetachableDockWidgets* getParentCandidateForDockWidget(
      DockWidget* dock_widget);
  static const std::set<MainWindowWithDetachableDockWidgets*>& getMainWindows();
  static MainWindowWithDetachableDockWidgets* getFirstMainWindow();
  static MainWindowWithDetachableDockWidgets* getOrCreateWindowForScreen(
      int screen);
  static MainWindowWithDetachableDockWidgets* getOwnerOfDockWidget(
        DockWidget* dock_widget);
  static void hideAllRubberBands();

 public slots:
  void dockLocationChanged(Qt::DockWidgetArea area);
  void tabCloseRequested(int index);
  void childAddedNotify(QObject* child);
  void updateDockWidgetTitleBars();
  void updateCloseButtonsOnTabBars();
  void updateActiveDockWidget();
  void updateDocksAndTabs();

 signals:
  void childAdded(QObject* child);
  void childRemoved();

 protected:
  bool event(QEvent* event) Q_DECL_OVERRIDE;
  bool splitDockWidgetImpl(QDockWidget* first, QDockWidget* second, Qt::Orientation orientation);

 private:
  static std::set<MainWindowWithDetachableDockWidgets*> main_windows_;
  static MainWindowWithDetachableDockWidgets* first_main_window_;
  static int last_created_window_id_;

  TabBarEventFilter* tab_bar_event_filter_;
  QRubberBand* rubber_band_;

  bool dock_widgets_with_no_title_bars_;
};

/*****************************************************************************/
/* VelesMainWindow */
/*****************************************************************************/

class VelesMainWindow : public MainWindowWithDetachableDockWidgets {
  Q_OBJECT

 public:
  VelesMainWindow();
  void addFile(QString path);
  QStringList parsersList() {return parsers_list_;}

 protected:
  void dropEvent(QDropEvent *ev) Q_DECL_OVERRIDE;
  void dragEnterEvent(QDragEnterEvent *ev) Q_DECL_OVERRIDE;

 private slots:
  void newFile();
  void open();
  void about();
  void updateParsers(dbif::PInfoReply replay);

 private:
  void init();
  void createActions();
  void createMenus();
  void createDb();
  void createFileBlob(QString);
  void createHexEditTab(QString, dbif::ObjectHandle);
  void createLogWindow();

  QMenu *fileMenu;
  QMenu *visualisationMenu;
  QMenu *helpMenu;

  QAction *newFileAct;
  QAction *openAct;
  QAction *exitAct;
  QAction *optionsAct;

  QAction *aboutAct;
  QAction *aboutQtAct;

  dbif::ObjectHandle database;
  OptionsDialog *optionsDialog;

  QStringList parsers_list_;
};

}  // namespace ui
}  // namespace veles

#endif
