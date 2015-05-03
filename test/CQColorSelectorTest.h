#include <QDialog>

class CQColorSelector;

class CQColorSelectorTest : public QDialog {
  Q_OBJECT

 public:
  CQColorSelectorTest();

 private:
  CQColorSelector *stroke_;
};
