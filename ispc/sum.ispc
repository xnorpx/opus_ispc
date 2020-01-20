export uniform float sum(uniform const float data[],
                         uniform const int dataSize) {
    float sum = 0.f;
    foreach (i = 0 ... dataSize) { sum += data[i]; }
    return reduce_add(sum);
}
