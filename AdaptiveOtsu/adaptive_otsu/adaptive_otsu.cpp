#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QTextStream>
#include <cmath>

using namespace std;

const int INTENS_MIN = 0;
const int INTENS_MAX = 255;
const double EPS = 1e-12;

const double ETA_THRESHOLD = 0.85;
const int MIN_SIZE = 32;

const QString file_name = "../face.jpg";

// -------------------------------------------------
// Histogram
// -------------------------------------------------
void calcHisto(const QImage &image, double histo[])
{
    for (int i = 0; i <= INTENS_MAX; i++)
        histo[i] = 0;

    for (int r = 0; r < image.height(); r++)
    {
        const quint8 *row = image.constScanLine(r);
        for (int c = 0; c < image.width(); c++)
            histo[row[c]]++;
    }

    double n = image.width() * image.height();
    for (int i = 0; i <= INTENS_MAX; i++)
        histo[i] /= n;
}

// -------------------------------------------------
// σ²ᵀ – обща дисперсия
// -------------------------------------------------
double totalVariance(const double histo[])
{
    double mean = 0;
    for (int i = 0; i <= INTENS_MAX; i++)
        mean += i * histo[i];

    double var = 0;
    for (int i = 0; i <= INTENS_MAX; i++)
        var += (i - mean) * (i - mean) * histo[i];

    return var;
}

// -------------------------------------------------
// σ²ᴮ(k) – междукласова дисперсия
// -------------------------------------------------
double betweenClassVariance(
    int k,
    const double histo[],
    const double p1[],
    const double m[],
    double mG)
{
    double div = p1[k] * (1.0 - p1[k]);
    if (div < EPS)
        return 0;

    return pow(mG * p1[k] - m[k], 2) / div;
}

// -------------------------------------------------
// Otsu + η
// -------------------------------------------------
int otsuWithEta(const double histo[], double &eta)
{
    double p1[256] = {0};
    double m[256] = {0};

    p1[0] = histo[0];
    for (int i = 1; i <= INTENS_MAX; i++)
        p1[i] = p1[i - 1] + histo[i];

    for (int i = 1; i <= INTENS_MAX; i++)
        m[i] = m[i - 1] + i * histo[i];

    double mG = m[INTENS_MAX];

    double maxBC = 0;
    int bestT = 0;

    for (int k = 1; k <= INTENS_MAX; k++)
    {
        double bc = betweenClassVariance(k, histo, p1, m, mG);
        if (bc > maxBC)
        {
            maxBC = bc;
            bestT = k;
        }
    }

    double sigmaT2 = totalVariance(histo);
    eta = (sigmaT2 < EPS) ? 0 : maxBC / sigmaT2;

    return bestT;
}

// -------------------------------------------------
// Threshold
// -------------------------------------------------
void applyThreshold(QImage &image, int t)
{
    for (int r = 0; r < image.height(); r++)
    {
        quint8 *row = image.scanLine(r);
        for (int c = 0; c < image.width(); c++)
            row[c] = (row[c] < t) ? INTENS_MIN : INTENS_MAX;
    }
}

// -------------------------------------------------
// Adaptive Otsu (recursive)
// -------------------------------------------------
QImage adaptiveOtsu(const QImage &image)
{
    if (image.width() < MIN_SIZE || image.height() < MIN_SIZE)
    {
        QImage out = image;
        double histo[256];
        calcHisto(out, histo);
        double eta;
        int t = otsuWithEta(histo, eta);
        applyThreshold(out, t);
        return out;
    }

    double histo[256];
    calcHisto(image, histo);

    double eta;
    int t = otsuWithEta(histo, eta);

    if (eta >= ETA_THRESHOLD)
    {
        QImage out = image;
        applyThreshold(out, t);
        return out;
    }

    int w2 = image.width() / 2;

    QImage left = image.copy(0, 0, w2, image.height());
    QImage right = image.copy(w2, 0, image.width() - w2, image.height());

    QImage leftRes = adaptiveOtsu(left);
    QImage rightRes = adaptiveOtsu(right);

    QImage result(image.size(), image.format());
    QPainter p(&result);
    p.drawImage(0, 0, leftRes);
    p.drawImage(w2, 0, rightRes);

    return result;
}

// -------------------------------------------------
// MAIN
// -------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QImage image;
    QLabel label;

    if (!image.load(file_name))
        return 0;

    if (image.format() != QImage::Format_Grayscale8)
        image = image.convertToFormat(QImage::Format_Grayscale8);

    QImage result = adaptiveOtsu(image);
    label.setPixmap(QPixmap::fromImage(result));
    label.show();

    return app.exec();
}
