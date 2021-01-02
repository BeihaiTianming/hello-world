#ifndef TEXTHIGHLIGHTER_H
#define TEXTHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class TextHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    TextHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat h1Format;
    QTextCharFormat h2Format;
    QTextCharFormat h3Format;
    QTextCharFormat h4Format;
    QTextCharFormat h5Format;
    QTextCharFormat listFormat;
};

#endif // TEXTHIGHLIGHTER_H
