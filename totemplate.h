//***************************************************************************
/* $Id$
**
** Copyright (C) 2000-2001 GlobeCom AB.  All rights reserved.
**
** This file is part of the Toolkit for Oracle.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.globecom.net/tora/ for more information.
**
** Contact tora@globecom.se if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef __TOTEMPLATE_H
#define __TOTEMPLATE_H

#include <list>
#include <map>

#include <qvbox.h>
#include "toresultview.h"
#include "tohelp.h"

class QListView;
class QListViewItem;
class QSplitter;
class QTextView;
class toConnection;
class toListView;
class toTemplateItem;
class toTemplateProvider;

/** Not part of the API.
 */

class toTemplate : public QVBox, public toHelpContext {
  Q_OBJECT

  QSplitter *Splitter;
  toListView *List;
  QWidget *WidgetExtra;
  QVBox *Frame;
public:
  toTemplate(QWidget *parent);
  virtual ~toTemplate();
  void setWidget(QWidget *widget);
  QWidget *widget(void)
  { return WidgetExtra; }
  QWidget *frame(void)
  { return Frame; }

  static QWidget *parentWidget(QListViewItem *item);
  static toTemplate *templateWidget(QListViewItem *item);
  static toTemplate *templateWidget(QListView *obj);

  virtual bool canHandle(toConnection &conn)
  { return true; }
public slots:
  void expand(QListViewItem *item);
  void collapse(QListViewItem *item);
};

/**
 * This is the base class of a provider of template items. Usually it is created with
 * a static object just like the tools.
 */

class toTemplateProvider {
  /** List of currently available template providers.
   */
  static std::list<toTemplateProvider *> *Providers;
public:
  toTemplateProvider();
  virtual ~toTemplateProvider()
  { }

  /** Insert a parent item for this template provider into a list view.
   * @param parent The list into which to insert the item.
   */
  virtual void insertItems(QListView *parent)=0;

  friend class toTemplate;
};

/** This is an item that is contained in a template. It is different in the expand and
 * collapse methods that are called when this item is expanded or collapsed from the
 * template widget.
 */
class toTemplateItem : public toResultViewItem {
  /** The provider responsible for this item.
   */
  toTemplateProvider &Provider;
public:
  /** Create an item.
   * @param prov Provider for this item.
   * @param parent Parent of this item.
   * @param name Contents of the first column of the item.
   */
  toTemplateItem(toTemplateProvider &prov,QListView *parent,const QString &name)
    : toResultViewItem(parent,NULL,name),Provider(prov)
  { }
  /** Create an item.
   * @param parent Parent of this item.
   * @param name Contents of the first column of the item.
   */
  toTemplateItem(toTemplateItem *parent,const QString &name)
    : toResultViewItem(parent,NULL,name),Provider(parent->provider())
  { }
  /** Get a reference to the provider of this item.
   * @return Reference to provider.
   */
  toTemplateProvider &provider(void)
  { return Provider; }
  /** This function is called when the item is expanded and can be used to fill up the
   * list when needed.
   */
  virtual void expand(void)
  { }
  /** This function is called when the item is collapsed.
   */
  virtual void collapse(void)
  { }
  /** This function can be used to return a widget that is displays extra information for
   * this item. Return NULL if no widget is to be displayed. The widget will be deleted
   * when this item is no longer shown.
   */
  virtual QWidget *selectedWidget(QWidget *parent)
  { return NULL; }
  /** Reimplemented for internal reasons.
   */
  virtual void setSelected(bool sel);
};

/** This class represent a template item that have an extra text to describe it if it is
 * selected.
 */

class toTemplateText : public toTemplateItem {
  /** The note to display.
   */
  const QString Note;
public:
  /** Create an item.
   * @param parent Parent of this item.
   * @param name Contents of the first column of the item.
   * @param note Extra text to display if item is selected.
   */
  toTemplateText(toTemplateItem *parent,const QString &name,const QString &note)
    : toTemplateItem(parent,name), Note(note)
  { }
  /** Reimplemented for internal reasons.
   */
  virtual QWidget *selectedWidget(QWidget *parent);
};

/** This class represent an item that when expanded will execute an SQL statement
 * and create child items which are the result of the query.
 */

class toTemplateSQL : public toTemplateItem {
  /** Connection to run statement in
   */
  toConnection &Connection;
  /** Statement to run.
   */
  QString SQL;
public:
  /** Create an item.
   * @param conn Connection to query.
   * @param parent Parent of this item.
   * @param name Contents of the first column of the item.
   * @param sql SQL statement, observe that it is in @ref QCString format and you
   *            should use utf8 if converting from QString.
   */
  toTemplateSQL(toConnection &conn,toTemplateItem *parent,
		const QString &name,const QString &sql)
    : toTemplateItem(parent,name),Connection(conn),SQL(sql)
  { setExpandable(true); }
  /** Get connection of this item.
   * @return Reference to connection.
   */
  toConnection &connection()
  { return Connection; }
  /** Create a child of this item.
   * @param name Name of the child.
   * @return A newly created item.
   */
  virtual toTemplateItem *createChild(const QString &name)
  { return new toTemplateItem(this,name); }
  /** Get parameters to pass to query.
   * @return List of strings to pass as input parameters to query
   */
  virtual toQList parameters(void)
  { toQList ret; return ret; }
  /** Reimplemented for internal reasons.
   */
  virtual void expand(void);
};

#endif
