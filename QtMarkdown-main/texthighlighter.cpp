#include "texthighlighter.h"

//! [0]
TextHighlighter::TextHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent){
    HighlightingRule rule;

    h1Format.setForeground(Qt::red);
    rule.pattern = QRegularExpression("# [^\n]*");
    rule.format = h1Format;
    highlightingRules.append(rule);

    h2Format.setForeground(Qt::darkYellow);
    rule.pattern = QRegularExpression("## [^\n]*");
    rule.format = h2Format;
    highlightingRules.append(rule);

    h3Format.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("### [^\n]*");
    rule.format = h3Format;
    highlightingRules.append(rule);

    h4Format.setForeground(Qt::darkCyan);
    rule.pattern = QRegularExpression("#### [^\n]*");
    rule.format = h4Format;
    highlightingRules.append(rule);

    h5Format.setForeground(Qt::darkBlue);
    rule.pattern = QRegularExpression("##### [^\n]*");
    rule.format = h5Format;
    highlightingRules.append(rule);

    listFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("- [^\n]*");
    rule.format = listFormat;
    highlightingRules.append(rule);
}

void TextHighlighter::highlightBlock(const QString &text) {
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
