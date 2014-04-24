/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package zm.client.jni;

public class zmGtdModel {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected zmGtdModel(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(zmGtdModel obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        javaZmJNI.delete_zmGtdModel(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public zmGtdModel() {
    this(javaZmJNI.new_zmGtdModel__SWIG_0(), true);
  }

  public zmGtdModel(MindMatterModel model) {
    this(javaZmJNI.new_zmGtdModel__SWIG_1(MindMatterModel.getCPtr(model), model), true);
  }

  public MindMatterModel const_base() {
    long cPtr = javaZmJNI.zmGtdModel_const_base(swigCPtr, this);
    return (cPtr == 0) ? null : new MindMatterModel(cPtr, true);
  }

  public MindMatterModel base() {
    long cPtr = javaZmJNI.zmGtdModel_base(swigCPtr, this);
    return (cPtr == 0) ? null : new MindMatterModel(cPtr, true);
  }

  public void initialize() {
    javaZmJNI.zmGtdModel_initialize(swigCPtr, this);
  }

  public String getNote(String uid) {
    return javaZmJNI.zmGtdModel_getNote(swigCPtr, this, uid);
  }

  public int getImportance(String uid) {
    return javaZmJNI.zmGtdModel_getImportance(swigCPtr, this, uid);
  }

  public StringVec getInboxItems(boolean includeDoneItems) {
    return new StringVec(javaZmJNI.zmGtdModel_getInboxItems(swigCPtr, this, includeDoneItems), true);
  }

  public StringVec getTaskItems(boolean includeStandaloneTasks, boolean includeDoneItems) {
    return new StringVec(javaZmJNI.zmGtdModel_getTaskItems(swigCPtr, this, includeStandaloneTasks, includeDoneItems), true);
  }

  public StringVec getProjectItems(boolean includeStandaloneTasks, boolean includeDoneItems) {
    return new StringVec(javaZmJNI.zmGtdModel_getProjectItems(swigCPtr, this, includeStandaloneTasks, includeDoneItems), true);
  }

  public StringVec getDoneItems() {
    return new StringVec(javaZmJNI.zmGtdModel_getDoneItems(swigCPtr, this), true);
  }

  public StringVec find(String pattern) {
    return new StringVec(javaZmJNI.zmGtdModel_find(swigCPtr, this, pattern), true);
  }

  public boolean isTaskItem(String item, boolean includeStandaloneTasks) {
    return javaZmJNI.zmGtdModel_isTaskItem(swigCPtr, this, item, includeStandaloneTasks);
  }

  public boolean isInboxItem(String item) {
    return javaZmJNI.zmGtdModel_isInboxItem(swigCPtr, this, item);
  }

  public boolean isProjectItem(String item, boolean includeStandaloneTasks) {
    return javaZmJNI.zmGtdModel_isProjectItem(swigCPtr, this, item, includeStandaloneTasks);
  }

  public boolean isDone(String task_item) {
    return javaZmJNI.zmGtdModel_isDone(swigCPtr, this, task_item);
  }

  public String getParentProject(String task_item) {
    return javaZmJNI.zmGtdModel_getParentProject(swigCPtr, this, task_item);
  }

  public String getNextTask(String task_item) {
    return javaZmJNI.zmGtdModel_getNextTask(swigCPtr, this, task_item);
  }

  public String orderATask() {
    return javaZmJNI.zmGtdModel_orderATask(swigCPtr, this);
  }

  public boolean itemContentMatchesString(String uid, String searchString) {
    return javaZmJNI.zmGtdModel_itemContentMatchesString(swigCPtr, this, uid, searchString);
  }

  public void print_statistics() {
    javaZmJNI.zmGtdModel_print_statistics(swigCPtr, this);
  }

  public boolean empty() {
    return javaZmJNI.zmGtdModel_empty(swigCPtr, this);
  }

  public String createNewInboxItem(String caption) {
    return javaZmJNI.zmGtdModel_createNewInboxItem(swigCPtr, this, caption);
  }

  public void setNote(String uid, String value) {
    javaZmJNI.zmGtdModel_setNote(swigCPtr, this, uid, value);
  }

  public void plusOne(String uid) {
    javaZmJNI.zmGtdModel_plusOne(swigCPtr, this, uid);
  }

  public void registerItemAsTask(String task_item, String project_item) {
    javaZmJNI.zmGtdModel_registerItemAsTask(swigCPtr, this, task_item, project_item);
  }

  public void setDone(String task_item) {
    javaZmJNI.zmGtdModel_setDone(swigCPtr, this, task_item);
  }

  public void castToProject(String item) {
    javaZmJNI.zmGtdModel_castToProject(swigCPtr, this, item);
  }

  public void castToStandaloneProject(String item) {
    javaZmJNI.zmGtdModel_castToStandaloneProject(swigCPtr, this, item);
  }

  public void castToTaggedItem(String item, String tag) {
    javaZmJNI.zmGtdModel_castToTaggedItem(swigCPtr, this, item, tag);
  }

  public void setNextTask(String project_item, String task_item) {
    javaZmJNI.zmGtdModel_setNextTask(swigCPtr, this, project_item, task_item);
  }

  public String createProject(String project_name) {
    return javaZmJNI.zmGtdModel_createProject(swigCPtr, this, project_name);
  }

}
