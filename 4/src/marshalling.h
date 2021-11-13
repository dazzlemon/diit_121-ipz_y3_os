#pragma once

#include <QList>
#include <QDataStream>

QList<int> deque_bytes(char* rawdata, size_t size_) {
    QList<int> d;
    auto bytes = QByteArray::fromRawData(rawdata, size_);
    QDataStream stream(&bytes, QIODevice::ReadOnly);
    stream >> d;
    return d;
}

QList<int> deque_bytes_(char* rawdata) {
    return deque_bytes(rawdata + 1, rawdata[0]);
}

std::pair<char*, size_t> bytes_deque(QList<int> d) {
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << d;
    
    char* res = new char[bytes.length()];
    memcpy(res, bytes.constData(), bytes.length());

    return {res, bytes.length()};
}

char* bytes_deque_(QList<int> d) {
    auto b_sz = bytes_deque(d);
    auto b_ = new char[b_sz.second + 1];
    b_[0] = b_sz.second;
    memcpy(b_ + 1, b_sz.first, b_sz.second);
    return b_;
}