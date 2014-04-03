/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package zm.client.jni;

public class StringVec {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected StringVec(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(StringVec obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        javaZmJNI.delete_StringVec(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public StringVec() {
    this(javaZmJNI.new_StringVec__SWIG_0(), true);
  }

  public StringVec(long n) {
    this(javaZmJNI.new_StringVec__SWIG_1(n), true);
  }

  public long size() {
    return javaZmJNI.StringVec_size(swigCPtr, this);
  }

  public long capacity() {
    return javaZmJNI.StringVec_capacity(swigCPtr, this);
  }

  public void reserve(long n) {
    javaZmJNI.StringVec_reserve(swigCPtr, this, n);
  }

  public boolean isEmpty() {
    return javaZmJNI.StringVec_isEmpty(swigCPtr, this);
  }

  public void clear() {
    javaZmJNI.StringVec_clear(swigCPtr, this);
  }

  public void add(String x) {
    javaZmJNI.StringVec_add(swigCPtr, this, x);
  }

  public String get(int i) {
    return javaZmJNI.StringVec_get(swigCPtr, this, i);
  }

  public void set(int i, String val) {
    javaZmJNI.StringVec_set(swigCPtr, this, i, val);
  }

}
