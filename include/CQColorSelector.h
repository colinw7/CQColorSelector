#ifndef CQColorSelector_H
#define CQColorSelector_H

#include <QSpinBox>
#include <QLineEdit>
#include <QToolButton>

class CQColorSpin;
class CQColorButton;
class CQColorEdit;
class CQColorGradient;
class CQColorSelectorWheel;
class QTabWidget;

//-----

class CQColorSelector : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QColor color READ color WRITE setColor)

 public:
  enum class ColorMode {
    RGB,
    HSL,
    CMYK,
    WHEEL
  };

  enum class ColorType {
    RGB_R,
    RGB_G,
    RGB_B,
    HSL_H,
    HSL_S,
    HSL_L,
    CMYK_C,
    CMYK_M,
    CMYK_Y,
    CMYK_K,
    ALPHA
  };

 public:
  CQColorSelector(QWidget *parent=0);

  const QColor &color() const { return c_; }

  void setColorType(ColorType type, int v);

  QSize sizeHint() const;

 public slots:
  void setColor(const QColor &c);

 signals:
  void colorChanged(const QColor &c);

 private slots:
  void tabChanged(int i);

 private:
  QWidget *createRGBTab();
  QWidget *createHSLTab();
  QWidget *createCMYKTab();
  QWidget *createWheelTab();

 private:
  struct RGBWidgets {
    CQColorGradient *rcanvas;
    CQColorGradient *gcanvas;
    CQColorGradient *bcanvas;
    CQColorGradient *acanvas;

    CQColorSpin *rspin;
    CQColorSpin *gspin;
    CQColorSpin *bspin;
    CQColorSpin *aspin;
  };

  struct HSLWidgets {
    CQColorGradient *hcanvas;
    CQColorGradient *scanvas;
    CQColorGradient *lcanvas;
    CQColorGradient *acanvas;

    CQColorSpin *hspin;
    CQColorSpin *sspin;
    CQColorSpin *lspin;
    CQColorSpin *aspin;
  };

  struct CMYKWidgets {
    CQColorGradient *ccanvas;
    CQColorGradient *mcanvas;
    CQColorGradient *ycanvas;
    CQColorGradient *kcanvas;
    CQColorGradient *acanvas;

    CQColorSpin *cspin;
    CQColorSpin *mspin;
    CQColorSpin *yspin;
    CQColorSpin *kspin;
    CQColorSpin *aspin;
  };

  struct WheelWidgets {
    CQColorSelectorWheel *wheel;
    CQColorGradient      *acanvas;
    CQColorSpin          *aspin;
  };

  QColor    c_;
  ColorMode mode_;

  QTabWidget *tab_;

  RGBWidgets   rgbw_;
  HSLWidgets   hslw_;
  CMYKWidgets  cmykw_;
  WheelWidgets wheel_;

  CQColorButton *colorButton_;
  CQColorEdit   *valueEdit_;
};

//-----

class CQColorGradient : public QWidget {
 public:
  typedef CQColorSelector::ColorType ColorType;

 public:
  CQColorGradient(CQColorSelector *stroke, ColorType type);

  void paintEvent(QPaintEvent *);

  void mousePressEvent  (QMouseEvent *e);
  void mouseMoveEvent   (QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

 private:
  CQColorSelector *stroke_;
  ColorType        type_;
};

//-----

class CQColorSelectorWheel : public QWidget {
 public:
  CQColorSelectorWheel(CQColorSelector *stroke);

  void paintEvent(QPaintEvent *);

  void mousePressEvent  (QMouseEvent *e);
  void mouseMoveEvent   (QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

 private:
  bool updateCircle  (int x, int y, bool updatePos);
  bool updateTriangle(int x, int y, bool updatePos);

  void angleToPoint(double r, double a, double &x, double &y);

  bool toBarycentric(double x, double y, double &b1, double &b2, double &b3);

  double triangleArea2(double x1, double y1, double x2, double y2, double x3, double y3);

  double pointLineDistance(double x, double y, double xl1, double yl1, double xl2, double yl2);

 private:
  CQColorSelector *stroke_;
  bool             circle_;
  bool             triangle_;
  int              pressX_, pressY_;
  double           ps_;
  double           xc_, yc_;
  double           ri_, ro_;
  double           xt1_, yt1_, xt2_, yt2_, xt3_, yt3_;
};

//-----

class CQColorSpin : public QSpinBox {
  Q_OBJECT

 public:
  typedef CQColorSelector::ColorType ColorType;

 public:
  CQColorSpin(CQColorSelector *stroke, ColorType type);

 private slots:
  void setColorSlot(int v);

 private:
  CQColorSelector *stroke_;
  ColorType        type_;
};

//-----

class CQColorButton : public QToolButton {
  Q_OBJECT

 public:
  CQColorButton(CQColorSelector *stroke, const QColor &c);

  void setColor(const QColor &c);

  void paintEvent(QPaintEvent *);

 private:
  CQColorSelector *stroke_;
  QColor           c_;
};

//-----

class CQColorEdit : public QLineEdit {
  Q_OBJECT

 public:
  CQColorEdit(CQColorSelector *stroke, const QColor &c);

  void setColor(const QColor &c);

 signals:
  void colorChanged(const QColor &c);

 private slots:
  void valueChangedSlot();

 private:
  CQColorSelector *stroke_;
  QColor           c_;
  QString          str_;
};

#endif
