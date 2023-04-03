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
  struct Config {
    Config() { }

    bool rgbTab      { true };
    bool hslTab      { true };
    bool cmykTab     { false };
    bool wheelTab    { true };
    bool alpha       { true };
    bool colorButton { true };
    bool colorEdit   { true };
  };

 public:
  CQColorSelector(QWidget *parent=nullptr, const Config &config=Config());

  const QColor &color() const { return c_; }

  void setColorType(ColorType type, int v);

  QSize sizeHint() const override;

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
    CQColorGradient *rcanvas { 0 };
    CQColorGradient *gcanvas { 0 };
    CQColorGradient *bcanvas { 0 };
    CQColorGradient *acanvas { 0 };

    CQColorSpin *rspin { 0 };
    CQColorSpin *gspin { 0 };
    CQColorSpin *bspin { 0 };
    CQColorSpin *aspin { 0 };
  };

  struct HSLWidgets {
    CQColorGradient *hcanvas { 0 };
    CQColorGradient *scanvas { 0 };
    CQColorGradient *lcanvas { 0 };
    CQColorGradient *acanvas { 0 };

    CQColorSpin *hspin { 0 };
    CQColorSpin *sspin { 0 };
    CQColorSpin *lspin { 0 };
    CQColorSpin *aspin { 0 };
  };

  struct CMYKWidgets {
    CQColorGradient *ccanvas { 0 };
    CQColorGradient *mcanvas { 0 };
    CQColorGradient *ycanvas { 0 };
    CQColorGradient *kcanvas { 0 };
    CQColorGradient *acanvas { 0 };

    CQColorSpin *cspin { 0 };
    CQColorSpin *mspin { 0 };
    CQColorSpin *yspin { 0 };
    CQColorSpin *kspin { 0 };
    CQColorSpin *aspin { 0 };
  };

  struct WheelWidgets {
    CQColorSelectorWheel *wheel   { 0 };
    CQColorGradient      *acanvas { 0 };
    CQColorSpin          *aspin   { 0 };
  };

  QColor    c_;
  ColorMode mode_;
  Config    config_;

  QTabWidget *tab_ { 0 };

  RGBWidgets   rgbw_;
  HSLWidgets   hslw_;
  CMYKWidgets  cmykw_;
  WheelWidgets wheel_;

  CQColorButton *colorButton_ { nullptr };
  CQColorEdit   *colorEdit_   { nullptr };
};

//-----

class CQColorGradient : public QWidget {
 public:
  typedef CQColorSelector::ColorType ColorType;

 public:
  CQColorGradient(CQColorSelector *stroke, ColorType type);

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

 private:
  CQColorSelector *stroke_ { nullptr };
  ColorType        type_;
};

//-----

class CQColorSelectorWheel : public QWidget {
 public:
  CQColorSelectorWheel(CQColorSelector *stroke);

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

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

  void paintEvent(QPaintEvent *) override;

 private:
  CQColorSelector *stroke_ { nullptr };
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
  CQColorSelector *stroke_ { nullptr };
  QColor           c_;
  QString          str_;
};

#endif
