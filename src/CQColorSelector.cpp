#include <CQColorSelector.h>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QPainter>
#include <QMouseEvent>
#include <iostream>

namespace {

inline double norm(double x, double low, double high) {
  return (x - low)/(high - low);
}

inline double lerp(double value1, double value2, double amt) {
  return value1 + (value2 - value1)*amt;
}

inline double map(double value, double low1, double high1, double low2, double high2) {
  return lerp(low2, high2, norm(value, low1, high1));
}

inline int imap(double value, double low1, double high1, double low2, double high2) {
  return int(lerp(low2, high2, norm(value, low1, high1)) + 0.5);
}

inline double clamp(double value, double l, double h) {
  return std::min(std::max(value, l), h);
}

QColor toBW(const QColor &c) {
  int g = qGray(c.red(), c.green(), c.blue());

  return (g < 128 ? QColor(255,255,255) : QColor(0,0,0));
}

void paintCheckerboard(QPainter *p, int px, int py, int pw, int ph, int s) {
  int h = ph/s; h += (ph % s == 0 ? 0 : 1);
  int w = pw/s; w += (pw % s == 0 ? 0 : 1);

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      QColor c = (((x + y) & 1) ? QColor(96,96,96) : QColor(160,160,160));

      p->fillRect(QRect(px + s*x, py + s*y, s, s), QBrush(c));
    }
  }
}

}

class CQColorLabel : public QLabel {
 public:
  CQColorLabel(const char *label) :
   QLabel(label) {
    setObjectName("label");

    QFontMetrics fm(font());

    setFixedWidth(fm.width("W"));

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  }
};

//---

CQColorSelector::
CQColorSelector(QWidget *parent) :
 QWidget(parent), mode_(ColorMode::RGB)
{
  setObjectName("selector");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  tab_ = new QTabWidget;
  tab_->setObjectName("tab");

  layout->addWidget(tab_);

  //---

  tab_->addTab(createRGBTab  (), "RGB"  );
  tab_->addTab(createHSLTab  (), "HSL"  );
  tab_->addTab(createCMYKTab (), "CMYK" );
  tab_->addTab(createWheelTab(), "Wheel");

  //---

  //layout->addStretch();

  //---

  QHBoxLayout *llayout = new QHBoxLayout;

  colorButton_ = new CQColorButton(this, c_);
  valueEdit_   = new CQColorEdit(this, c_);

  llayout->addWidget(colorButton_);
  llayout->addStretch();
  llayout->addWidget(new QLabel("RGBA"));
  llayout->addWidget(valueEdit_);

  layout->addLayout(llayout);

  //---

  setColor(c_);

  //---

  connect(tab_, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

  connect(valueEdit_, SIGNAL(colorChanged(const QColor &)), this, SLOT(setColor(const QColor &)));
}

QWidget *
CQColorSelector::
createRGBTab()
{
  QWidget *tab = new QWidget;
  tab->setObjectName("rgb");

  QVBoxLayout *layout = new QVBoxLayout(tab);
  layout->setMargin(2); layout->setSpacing(2);

  QHBoxLayout *rlayout = new QHBoxLayout; rlayout->setSpacing(2);
  QHBoxLayout *glayout = new QHBoxLayout; glayout->setSpacing(2);
  QHBoxLayout *blayout = new QHBoxLayout; blayout->setSpacing(2);
  QHBoxLayout *alayout = new QHBoxLayout; alayout->setSpacing(2);

  rlayout->addWidget(new CQColorLabel("R"));
  glayout->addWidget(new CQColorLabel("G"));
  blayout->addWidget(new CQColorLabel("B"));
  alayout->addWidget(new CQColorLabel("A"));

  rlayout->addWidget(rgbw_.rcanvas = new CQColorGradient(this, ColorType::RGB_R));
  glayout->addWidget(rgbw_.gcanvas = new CQColorGradient(this, ColorType::RGB_G));
  blayout->addWidget(rgbw_.bcanvas = new CQColorGradient(this, ColorType::RGB_B));
  alayout->addWidget(rgbw_.acanvas = new CQColorGradient(this, ColorType::ALPHA));

  rlayout->addWidget(rgbw_.rspin = new CQColorSpin(this, ColorType::RGB_R));
  glayout->addWidget(rgbw_.gspin = new CQColorSpin(this, ColorType::RGB_G));
  blayout->addWidget(rgbw_.bspin = new CQColorSpin(this, ColorType::RGB_B));
  alayout->addWidget(rgbw_.aspin = new CQColorSpin(this, ColorType::ALPHA));

  layout->addLayout(rlayout);
  layout->addLayout(glayout);
  layout->addLayout(blayout);
  layout->addLayout(alayout);
  layout->addStretch();

  return tab;
}

QWidget *
CQColorSelector::
createHSLTab()
{
  QWidget *tab = new QWidget;
  tab->setObjectName("hsl");

  QVBoxLayout *layout = new QVBoxLayout(tab);
  layout->setMargin(2); layout->setSpacing(2);

  QHBoxLayout *hlayout = new QHBoxLayout; hlayout->setSpacing(2);
  QHBoxLayout *slayout = new QHBoxLayout; slayout->setSpacing(2);
  QHBoxLayout *llayout = new QHBoxLayout; llayout->setSpacing(2);
  QHBoxLayout *alayout = new QHBoxLayout; alayout->setSpacing(2);

  hlayout->addWidget(new CQColorLabel("H"));
  slayout->addWidget(new CQColorLabel("S"));
  llayout->addWidget(new CQColorLabel("L"));
  alayout->addWidget(new CQColorLabel("A"));

  hlayout->addWidget(hslw_.hcanvas = new CQColorGradient(this, ColorType::HSL_H));
  slayout->addWidget(hslw_.scanvas = new CQColorGradient(this, ColorType::HSL_S));
  llayout->addWidget(hslw_.lcanvas = new CQColorGradient(this, ColorType::HSL_L));
  alayout->addWidget(hslw_.acanvas = new CQColorGradient(this, ColorType::ALPHA));

  hlayout->addWidget(hslw_.hspin = new CQColorSpin(this, ColorType::HSL_H));
  slayout->addWidget(hslw_.sspin = new CQColorSpin(this, ColorType::HSL_S));
  llayout->addWidget(hslw_.lspin = new CQColorSpin(this, ColorType::HSL_L));
  alayout->addWidget(hslw_.aspin = new CQColorSpin(this, ColorType::ALPHA));

  layout->addLayout(hlayout);
  layout->addLayout(slayout);
  layout->addLayout(llayout);
  layout->addLayout(alayout);
  layout->addStretch();

  return tab;
}

QWidget *
CQColorSelector::
createCMYKTab()
{
  QWidget *tab = new QWidget;
  tab->setObjectName("cmyk");

  QVBoxLayout *layout = new QVBoxLayout(tab);
  layout->setMargin(2); layout->setSpacing(2);

  QHBoxLayout *clayout = new QHBoxLayout; clayout->setSpacing(2);
  QHBoxLayout *mlayout = new QHBoxLayout; mlayout->setSpacing(2);
  QHBoxLayout *ylayout = new QHBoxLayout; ylayout->setSpacing(2);
  QHBoxLayout *klayout = new QHBoxLayout; klayout->setSpacing(2);
  QHBoxLayout *alayout = new QHBoxLayout; alayout->setSpacing(2);

  clayout->addWidget(new CQColorLabel("C"));
  mlayout->addWidget(new CQColorLabel("M"));
  ylayout->addWidget(new CQColorLabel("Y"));
  klayout->addWidget(new CQColorLabel("K"));
  alayout->addWidget(new CQColorLabel("A"));

  clayout->addWidget(cmykw_.ccanvas = new CQColorGradient(this, ColorType::CMYK_C));
  mlayout->addWidget(cmykw_.mcanvas = new CQColorGradient(this, ColorType::CMYK_M));
  ylayout->addWidget(cmykw_.ycanvas = new CQColorGradient(this, ColorType::CMYK_Y));
  klayout->addWidget(cmykw_.kcanvas = new CQColorGradient(this, ColorType::CMYK_K));
  alayout->addWidget(cmykw_.acanvas = new CQColorGradient(this, ColorType::ALPHA ));

  clayout->addWidget(cmykw_.cspin = new CQColorSpin(this, ColorType::CMYK_C));
  mlayout->addWidget(cmykw_.mspin = new CQColorSpin(this, ColorType::CMYK_M));
  ylayout->addWidget(cmykw_.yspin = new CQColorSpin(this, ColorType::CMYK_Y));
  klayout->addWidget(cmykw_.kspin = new CQColorSpin(this, ColorType::CMYK_K));
  alayout->addWidget(cmykw_.aspin = new CQColorSpin(this, ColorType::ALPHA ));

  layout->addLayout(clayout);
  layout->addLayout(mlayout);
  layout->addLayout(ylayout);
  layout->addLayout(klayout);
  layout->addLayout(alayout);
  layout->addStretch();

  return tab;
}

QWidget *
CQColorSelector::
createWheelTab()
{
  QWidget *tab = new QWidget;
  tab->setObjectName("wheel");

  QVBoxLayout *layout = new QVBoxLayout(tab);
  layout->setMargin(2); layout->setSpacing(2);

  wheel_.wheel = new CQColorSelectorWheel(this);

  layout->addWidget(wheel_.wheel);

  QHBoxLayout *alayout = new QHBoxLayout; alayout->setSpacing(2);

  alayout->addWidget(new CQColorLabel("A"));
  alayout->addWidget(wheel_.acanvas = new CQColorGradient(this, ColorType::ALPHA));
  alayout->addWidget(wheel_.aspin   = new CQColorSpin (this, ColorType::ALPHA));

  layout->addLayout(alayout);

  return tab;
}

void
CQColorSelector::
setColor(const QColor &c)
{
  c_ = c;

  //---

  if      (mode_ == ColorMode::RGB) {
    rgbw_.rcanvas->update();
    rgbw_.gcanvas->update();
    rgbw_.bcanvas->update();
    rgbw_.acanvas->update();

    rgbw_.rspin->setValue(c_.red  ());
    rgbw_.gspin->setValue(c_.green());
    rgbw_.bspin->setValue(c_.blue ());
    rgbw_.aspin->setValue(c_.alpha());
  }
  else if (mode_ == ColorMode::HSL) {
    double h, s, l, a;

    c_.getHslF(&h, &s, &l, &a);

    hslw_.hcanvas->update();
    hslw_.scanvas->update();
    hslw_.lcanvas->update();
    hslw_.acanvas->update();

    hslw_.hspin->setValue(imap(h, 0, 1, 0, 255));
    hslw_.sspin->setValue(imap(s, 0, 1, 0, 255));
    hslw_.lspin->setValue(imap(l, 0, 1, 0, 255));
    hslw_.aspin->setValue(imap(a, 0, 1, 0, 255));
  }
  else if (mode_ == ColorMode::CMYK) {
    double c, m, y, k, a;

    c_.getCmykF(&c, &m, &y, &k, &a);

    cmykw_.ccanvas->update();
    cmykw_.mcanvas->update();
    cmykw_.ycanvas->update();
    cmykw_.kcanvas->update();
    cmykw_.acanvas->update();

    cmykw_.cspin->setValue(imap(c, 0, 1, 0, 255));
    cmykw_.mspin->setValue(imap(m, 0, 1, 0, 255));
    cmykw_.yspin->setValue(imap(y, 0, 1, 0, 255));
    cmykw_.kspin->setValue(imap(k, 0, 1, 0, 255));
    cmykw_.aspin->setValue(imap(a, 0, 1, 0, 255));
  }
  else if (mode_ == ColorMode::WHEEL) {
    double h, s, l, a;

    c_.getHslF(&h, &s, &l, &a);

    wheel_.wheel  ->update();
    wheel_.acanvas->update();

    wheel_.aspin->setValue(imap(a, 0, 1, 0, 255));
  }

  //---

  colorButton_->setColor(c_);
  valueEdit_  ->setColor(c_);

  //---

  emit colorChanged(c_);
}

void
CQColorSelector::
setColorType(ColorType type, int v)
{
  v = clamp(v, 0, 255);

  double rv = map(v, 0, 255, 0, 1);

  QColor qc = this->color();

  if      (type == ColorType::RGB_R) {
    qc.setRed(v);
  }
  else if (type == ColorType::RGB_G) {
    qc.setGreen(v);
  }
  else if (type == ColorType::RGB_B) {
    qc.setBlue(v);
  }
  else if (type == ColorType::HSL_H) {
    double h, s, l, a;

    qc.getHslF(&h, &s, &l, &a);

    qc.setHslF(rv, s, l, a);
  }
  else if (type == ColorType::HSL_S) {
    double h, s, l, a;

    qc.getHslF(&h, &s, &l, &a);

    qc.setHslF(h, rv, l, a);
  }
  else if (type == ColorType::HSL_L) {
    double h, s, l, a;

    qc.getHslF(&h, &s, &l, &a);

    qc.setHslF(h, s, rv, a);
  }
  else if (type == ColorType::CMYK_C) {
    double c, m, y, k, a;

    qc.getCmykF(&c, &m, &y, &k, &a);

    qc.setCmykF(rv, m, y, k, a);
  }
  else if (type == ColorType::CMYK_M) {
    double c, m, y, k, a;

    qc.getCmykF(&c, &m, &y, &k, &a);

    qc.setCmykF(c, rv, y, k, a);
  }
  else if (type == ColorType::CMYK_Y) {
    double c, m, y, k, a;

    qc.getCmykF(&c, &m, &y, &k, &a);

    qc.setCmykF(c, m, rv, k, a);
  }
  else if (type == ColorType::CMYK_K) {
    double c, m, y, k, a;

    qc.getCmykF(&c, &m, &y, &k, &a);

    qc.setCmykF(c, m, y, rv, a);
  }
  else if (type == ColorType::ALPHA) {
    qc.setAlpha(v);
  }

  this->setColor(qc);
}

void
CQColorSelector::
tabChanged(int i)
{
  if      (i == 0) mode_ = ColorMode::RGB;
  else if (i == 1) mode_ = ColorMode::HSL;
  else if (i == 2) mode_ = ColorMode::CMYK;
  else if (i == 3) mode_ = ColorMode::WHEEL;

  setColor(c_);
}

QSize
CQColorSelector::
sizeHint() const
{
  return QSize(250, 200);
}

//------

namespace {
void drawIndicatorUp(QPainter *p, int x, int y) {
  p->setRenderHint(QPainter::Antialiasing);

  int x1 = x - 4;
  int x2 = x + 4;
  int y1 = y;
  int y2 = y - 4;;

  QPainterPath path;

  path.moveTo(x1, y1);
  path.lineTo(x , y2);
  path.lineTo(x2, y1);

  path.closeSubpath();

  p->setPen(QColor(255,255,255));
  p->setBrush(QColor(0,0,0));

  p->drawPath(path);
}

void drawIndicatorDown(QPainter *p, int x, int y) {
  p->setRenderHint(QPainter::Antialiasing);

  int x1 = x - 4;
  int x2 = x + 4;
  int y1 = y;
  int y2 = y + 4;;

  QPainterPath path;

  path.moveTo(x1, y1);
  path.lineTo(x , y2);
  path.lineTo(x2, y1);

  path.closeSubpath();

  p->setPen(QColor(255,255,255));
  p->setBrush(QColor(0,0,0));

  p->drawPath(path);
}

void drawIndicators(QPainter *p, int x, int h) {
  drawIndicatorUp  (p, x, h - 1);
  drawIndicatorDown(p, x, 0    );
}

int pixelToColor(int x, int w) {
  return int(255.0*(1.0*x)/(w - 1) + 0.5);
}

}

//------

CQColorGradient::
CQColorGradient(CQColorSelector *stroke, ColorType type) :
 stroke_(stroke), type_(type)
{
  setObjectName("gradient");
}

void
CQColorGradient::
mousePressEvent(QMouseEvent *e)
{
  int v = pixelToColor(e->pos().x(), width());

  stroke_->setColorType(type_, v);
}

void
CQColorGradient::
mouseMoveEvent(QMouseEvent *e)
{
  int v = pixelToColor(e->pos().x(), width());

  stroke_->setColorType(type_, v);
}

void
CQColorGradient::
mouseReleaseEvent(QMouseEvent *e)
{
  int v = pixelToColor(e->pos().x(), width());

  stroke_->setColorType(type_, v);
}

void
CQColorGradient::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  QColor qc = stroke_->color();

  int pw = width ();
  int ph = height();

  if      (type_ == ColorType::RGB_R) {
    for (int x = 0; x < pw; ++x) {
      double r = (1.0*x)/(pw - 1);

      QColor c1(int(r*255 + 0.5), qc.green(), qc.blue());

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    int x = int((qc.red()*(pw - 1.0))/255.0 + 0.5);

    drawIndicators(&p, x, ph);
  }
  else if (type_ == ColorType::RGB_G) {
    for (int x = 0; x < pw; ++x) {
      double g = (1.0*x)/(pw - 1);

      QColor c1(qc.red(), int(g*255 + 0.5), qc.blue());

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    int x = int((qc.green()*(pw - 1.0))/255.0 + 0.5);

    drawIndicators(&p, x, ph);
  }
  else if (type_ == ColorType::RGB_B) {
    for (int x = 0; x < pw; ++x) {
      double b = (1.0*x)/(pw - 1);

      QColor c1(qc.red(), qc.green(), int(b*255 + 0.5));

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    int x = int((qc.blue()*(pw - 1.0))/255.0 + 0.5);

    drawIndicators(&p, x, ph);
  }
  else if (type_ == ColorType::HSL_H) {
    double h, s, l, a;

    qc.getHslF(&h, &s, &l, &a);

    for (int x = 0; x < pw; ++x) {
      QColor c1 = QColor::fromHslF(map(x, 0, pw - 1, 0, 1), 1, 0.5);

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    drawIndicators(&p, imap(h, 0, 1, 0, pw - 1), ph);
  }
  else if (type_ == ColorType::HSL_S) {
    double h, s, l, a;

    qc.getHslF(&h, &s, &l, &a);

    for (int x = 0; x < pw; ++x) {
      QColor c1 = QColor::fromHslF(h, map(x, 0, pw - 1, 0, 1), l);

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    drawIndicators(&p, imap(s, 0, 1, 0, pw - 1), ph);
  }
  else if (type_ == ColorType::HSL_L) {
    double h, s, l, a;

    qc.getHslF(&h, &s, &l, &a);

    for (int x = 0; x < pw; ++x) {
      QColor c1 = QColor::fromHslF(h, s, map(x, 0, pw - 1, 0, 1));

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    drawIndicators(&p, imap(l, 0, 1, 0, pw - 1), ph);
  }
  else if (type_ == ColorType::CMYK_C) {
    double c, m, y, k, a;

    qc.getCmykF(&c, &m, &y, &k, &a);

    for (int x = 0; x < pw; ++x) {
      QColor c1 = QColor::fromCmykF(map(x, 0, pw - 1, 0, 1), m, y, k);

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    drawIndicators(&p, imap(c, 0, 1, 0, pw - 1), ph);
  }
  else if (type_ == ColorType::CMYK_M) {
    double c, m, y, k, a;

    qc.getCmykF(&c, &m, &y, &k, &a);

    for (int x = 0; x < pw; ++x) {
      QColor c1 = QColor::fromCmykF(c, map(x, 0, pw - 1, 0, 1), y, k);

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    drawIndicators(&p, imap(m, 0, 1, 0, pw - 1), ph);
  }
  else if (type_ == ColorType::CMYK_Y) {
    double c, m, y, k, a;

    qc.getCmykF(&c, &m, &y, &k, &a);

    for (int x = 0; x < pw; ++x) {
      QColor c1 = QColor::fromCmykF(c, m, map(x, 0, pw - 1, 0, 1), k);

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    drawIndicators(&p, imap(y, 0, 1, 0, pw - 1), ph);
  }
  else if (type_ == ColorType::CMYK_K) {
    double c, m, y, k, a;

    qc.getCmykF(&c, &m, &y, &k, &a);

    for (int x = 0; x < pw; ++x) {
      QColor c1 = QColor::fromCmykF(c, m, y, map(x, 0, pw - 1, 0, 1));

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    drawIndicators(&p, imap(k, 0, 1, 0, pw - 1), ph);
  }
  else if (type_ == ColorType::ALPHA) {
    paintCheckerboard(&p, 0, 0, pw, ph, 7);

#if 0
    int s = 7;

    int h1 = ph/s; h1 += (ph % s == 0 ? 0 : 1);
    int w1 = pw/s; w1 += (pw % s == 0 ? 0 : 1);

    for (int y = 0; y < h1; ++y) {
      for (int x = 0; x < w1; ++x) {
        QColor c1 = (((x + y) & 1) ? QColor(96,96,96) : QColor(160,160,160));

        p.fillRect(QRect(s*x, s*y, s, s), QBrush(c1));
      }
    }
#endif

    for (int x = 0; x < pw; ++x) {
      double a = (1.0*x)/(pw - 1);

      QColor c1(qc.red(), qc.green(), qc.blue(), int(a*255 + 0.5));

      p.setPen(c1);

      p.drawLine(x, 0, x, ph - 1);
    }

    int x = int((qc.alpha()*(pw - 1.0))/255.0 + 0.5);

    drawIndicators(&p, x, ph);
  }
}

//------

CQColorSelectorWheel::
CQColorSelectorWheel(CQColorSelector *stroke) :
 stroke_(stroke), circle_(false), triangle_(false)
{
  setObjectName("wheel");
}

void
CQColorSelectorWheel::
mousePressEvent(QMouseEvent *e)
{
  circle_   = false;
  triangle_ = false;
  pressX_   = e->pos().x();
  pressY_   = e->pos().y();

  if (updateCircle(pressX_, pressY_, true)) {
    circle_ = true;
    return;
  }

  if (updateTriangle(pressX_, pressY_, true)) {
    triangle_ = true;
    return;
  }
}

void
CQColorSelectorWheel::
mouseMoveEvent(QMouseEvent *e)
{
  pressX_ = e->pos().x();
  pressY_ = e->pos().y();

  if (circle_)
    updateCircle(pressX_, pressY_, false);

  if (triangle_)
    updateTriangle(pressX_, pressY_, false);
}

void
CQColorSelectorWheel::
mouseReleaseEvent(QMouseEvent *e)
{
  pressX_ = e->pos().x();
  pressY_ = e->pos().y();

  if (circle_)
    updateCircle(pressX_, pressY_, false);

  if (triangle_)
    updateTriangle(pressX_, pressY_, false);
}

bool
CQColorSelectorWheel::
updateCircle(int x, int y, bool checkInside)
{
  double y1 = ps_ - 1 - y;

  double dx = x - xc_;
  double dy = y1 - yc_;

  double r = sqrt(dx*dx + dy*dy);

  if (checkInside && (r < ri_ || r > ro_)) {
    // TODO: move point inside if not check insde
    return false;
  }

  double a1 = atan2(dy, dx);

  if (a1 < 0) a1 = 2*M_PI + a1;

  double hue = 0.5*a1/M_PI;

  //---

  QColor qc = stroke_->color();

  double h, s, l, a;

  qc.getHslF(&h, &s, &l, &a);

  qc.setHslF(hue, s, l, a);

  stroke_->setColor(qc);

  return true;
}

bool
CQColorSelectorWheel::
updateTriangle(int x, int y, bool /*checkInside*/)
{
  double b1, b2, b3;

  if (! toBarycentric(x, y, b1, b2, b3))
    return false;

  b1 = clamp(b1, 0, 1);
  b2 = clamp(b2, 0, 1);
  b3 = clamp(b3, 0, 1);

  //---

  QColor qc = stroke_->color();

  double h, s, l, a;

  qc.getHslF(&h, &s, &l, &a);

  s = clamp(b2         , 0.0, 1.0);
  l = clamp(b2*0.5 + b1, 0.0, 1.0);

  qc.setHslF(h, s, l, a);

  stroke_->setColor(qc);

  return true;
}

void
CQColorSelectorWheel::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  int pw = width ();
  int ph = height();

  ps_ = std::min(pw, ph);

  //---

  QColor qc = stroke_->color();

  double h, s, l, a;

  qc.getHslF(&h, &s, &l, &a);

  //---

  // draw wheel at center (xc_, yc_), inner radius (ri), out radius (ro)
  ro_ = ps_/2.0;
  ri_ = ro_*0.75;

  xc_ = ro_;
  yc_ = ro_;

  for (int y = 0; y < ps_; ++y) {
    double y1 = ps_ - 1 - y;

    double dy = y1 - yc_;

    for (int x = 0; x < ps_; ++x) {
      double dx = x - xc_;

      double r = sqrt(dx*dx + dy*dy);

      if (r < ri_ || r > ro_)
        continue;

      double a = atan2(dy, dx);

      if (a < 0) a = 2*M_PI + a;

      double hue = 0.5*a/M_PI;

      QColor c;

      c.setHslF(hue, 1, 0.5);

      p.setPen(c);

      p.drawPoint(x, y);
    }
  }

  //---

  double la = h*M_PI*2;

  double x1, y1, x2, y2;

  angleToPoint(ri_, la, x1, y1);
  angleToPoint(ro_, la, x2, y2);

  QColor lc;

  lc.setHslF(h, 1, 0.5);

  p.setPen(toBW(lc));

  p.drawLine(x1, y1, x2, y2);

  //---

  // calc triangle corners
  //
  // p1 (xt1_, yt1_) is s=1,v=0.5
  // p2 (xt2_, yt2_) is s=0,v=0
  // p3 (xt3_, yt3_) is s=0,v=1

  angleToPoint(ri_, la           , xt1_, yt1_);
  angleToPoint(ri_, la + 2*M_PI/3, xt2_, yt2_);
  angleToPoint(ri_, la + 4*M_PI/3, xt3_, yt3_);

  // fill triangle
  int pxmin = int(std::min(std::min(xt1_, xt2_), xt3_));
  int pymin = int(std::min(std::min(yt1_, yt2_), yt3_));
  int pxmax = int(std::max(std::max(xt1_, xt2_), xt3_) + 0.9999);
  int pymax = int(std::max(std::max(yt1_, yt2_), yt3_) + 0.9999);

  int    minX = pressX_;
  int    minY = pressY_;
  double minD = 999;

  for (int y = pymin; y <= pymax; ++y) {
    //double y1 = ps_ - 1 - y;

    for (int x = pxmin; x <= pxmax; ++x) {
      double b1, b2, b3;

      if (! toBarycentric(x, y, b1, b2, b3))
        continue;

      double s1 = b2;
      double l1 = b2*0.5 + b1;

      QColor c;

      c.setHslF(h, s1, l1);

      p.setPen(c);

      p.drawPoint(x, y);

      double d = hypot(s1 - s, l1 - l);

      if (d < minD) {
        minD = d;
        minX = x;
        minY = y;
      }
    }
  }

  p.setPen(toBW(qc));

  p.drawEllipse(QRect(minX - 3, minY - 3, 6, 6));
}

double
CQColorSelectorWheel::
pointLineDistance(double x, double y, double xl1, double yl1, double xl2, double yl2)
{
  //double plx = x - xl1;
  //double ply = y - yl1;

  double dlx = xl2 - xl1;
  double dly = yl2 - yl1;

  double l = hypot(dlx, dly);

  double dot = xl2*yl1 - yl2*xl1;

  double d = fabs(dly*x - dlx*y + dot)/l;

  return d;
}

bool
CQColorSelectorWheel::
toBarycentric(double x, double y, double &b1, double &b2, double &b3)
{
  double area21 = triangleArea2(xt1_, yt1_, xt2_, yt2_, x, y);
  double area22 = triangleArea2(xt2_, yt2_, xt3_, yt3_, x, y);
  double area23 = triangleArea2(xt3_, yt3_, xt1_, yt1_, x, y);

  bool s1 = (area21 < 0.0);
  bool s2 = (area22 < 0.0);
  bool s3 = (area23 < 0.0);

  bool inside = ((s1 == s2) && (s2 == s3));

  double area1 = 0.5*fabs(area21);
  double area2 = 0.5*fabs(area22);
  double area3 = 0.5*fabs(area23);

  double is = 1.0/(area1 + area2 + area3);

  b1 = area1*is;
  b2 = area2*is;
  b3 = area3*is;

  return inside;
}

double
CQColorSelectorWheel::
triangleArea2(double x1, double y1, double x2, double y2, double x3, double y3)
{
  return (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);
}

void
CQColorSelectorWheel::
angleToPoint(double r, double a, double &x, double &y)
{
  x = xc_ + r*cos(a);
  y = yc_ + r*sin(a);

  y = ps_ - 1 - y;
}

//------

CQColorSpin::
CQColorSpin(CQColorSelector *stroke, ColorType type) :
 QSpinBox(), stroke_(stroke), type_(type)
{
  setObjectName("spin");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  setRange(0, 255);

  connect(this, SIGNAL(valueChanged(int)), this, SLOT(setColorSlot(int)));
}

void
CQColorSpin::
setColorSlot(int v)
{
  stroke_->setColorType(type_, v);
}

//------

CQColorEdit::
CQColorEdit(CQColorSelector *stroke, const QColor &c) :
 stroke_(stroke), c_(c)
{
  setObjectName("edit");

  setColor(c);

  connect(this, SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
}

void
CQColorEdit::
setColor(const QColor &c)
{
  c_ = c;

  QChar pad('0');

  str_ = QString("#%1%2%3%4").
    arg(c_.red (), 2, 16, pad).arg(c_.green(), 2, 16, pad).
    arg(c_.blue(), 2, 16, pad).arg(c_.alpha(), 2, 16, pad);

  setText(str_);
}

void
CQColorEdit::
valueChangedSlot()
{
  QString s = text().simplified();

  if (s == str_)
    return;

  QColor c;

  if (s.length() > 1 && s[0] == '#') {
    if      (s.length() == 9) {
      bool ok;

      int r = s.mid(1, 2).toInt(&ok, 16); if (! ok) return;
      int g = s.mid(3, 2).toInt(&ok, 16); if (! ok) return;
      int b = s.mid(5, 2).toInt(&ok, 16); if (! ok) return;
      int a = s.mid(7, 2).toInt(&ok, 16); if (! ok) return;

      c.setRgb(r, g, b, a);
    }
    else if (s.length() == 7) {
      bool ok;

      int r = s.mid(1, 2).toInt(&ok, 16); if (! ok) return;
      int g = s.mid(3, 2).toInt(&ok, 16); if (! ok) return;
      int b = s.mid(5, 2).toInt(&ok, 16); if (! ok) return;

      c.setRgb(r, g, b);
    }
    else if (s.length() == 5) {
      bool ok;

      int r = s.mid(1, 1).toInt(&ok, 16); if (! ok) return;
      int g = s.mid(2, 1).toInt(&ok, 16); if (! ok) return;
      int b = s.mid(3, 1).toInt(&ok, 16); if (! ok) return;
      int a = s.mid(4, 1).toInt(&ok, 16); if (! ok) return;

      c.setRgb(r*16 + r, g*16 + g, b*16 + b, a*16 + a);
    }
    else if (s.length() == 4) {
      bool ok;

      int r = s.mid(1, 1).toInt(&ok, 16); if (! ok) return;
      int g = s.mid(2, 1).toInt(&ok, 16); if (! ok) return;
      int b = s.mid(3, 1).toInt(&ok, 16); if (! ok) return;

      c.setRgb(r*16 + r, g*16 + g, b*16 + b);
    }
  }
  else {
    c = QColor(s);
  }

  if (! c.isValid())
    return;

  str_ = s;

  setColor(c);

  emit colorChanged(c_);
}

//------

CQColorButton::
CQColorButton(CQColorSelector *stroke, const QColor &c) :
 stroke_(stroke), c_(c)
{
  setObjectName("button");

  setColor(c);
}

void
CQColorButton::
setColor(const QColor &c)
{
  c_ = c;

  update();
}

void
CQColorButton::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  paintCheckerboard(&painter, 0, 0, width(), height(), 7);

  QBrush brush(c_);

  painter.fillRect(rect(), brush);
}
