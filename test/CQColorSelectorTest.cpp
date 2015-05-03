#include <CQColorSelectorTest.h>
#include <CQColorSelector.h>

#include <QApplication>
#include <QHBoxLayout>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  CQColorSelectorTest *test = new CQColorSelectorTest;

  test->resize(400, 300);

  test->show();

  return app.exec();
}

CQColorSelectorTest::
CQColorSelectorTest()
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  stroke_ = new CQColorSelector;

  layout->addWidget(stroke_);
}
