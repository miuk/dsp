#include <QLabel>
#include <QSlider>

class Slider : public QObject {
    Q_OBJECT
public:
    Slider(QWidget* parent, int rangeMin, int rangeMax
           , int initValue, int step=1);
    virtual ~Slider(void);
    void addToLayout(QLayout* layout);

public slots:
    void setValue(int val);
    void setRange(int rangeMin, int rangeMax);
signals:
    void valueChanged(int val);
private slots:
    void onValueChanged(int val);
signals:
    void setLabel(const QString&);
private:
    static int calcLabelCols(int rangeMin, int rangeMax);
private:
    QWidget* parent;
    QLabel* l;
    QSlider* s;
    int label_cols;
    int step;
};
