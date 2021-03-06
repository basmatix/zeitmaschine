/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package zm.client.jni;

public class javaZmJNI {
  public final static native long new_StringVec__SWIG_0();
  public final static native long new_StringVec__SWIG_1(long jarg1);
  public final static native long StringVec_size(long jarg1, StringVec jarg1_);
  public final static native long StringVec_capacity(long jarg1, StringVec jarg1_);
  public final static native void StringVec_reserve(long jarg1, StringVec jarg1_, long jarg2);
  public final static native boolean StringVec_isEmpty(long jarg1, StringVec jarg1_);
  public final static native void StringVec_clear(long jarg1, StringVec jarg1_);
  public final static native void StringVec_add(long jarg1, StringVec jarg1_, String jarg2);
  public final static native String StringVec_get(long jarg1, StringVec jarg1_, int jarg2);
  public final static native void StringVec_set(long jarg1, StringVec jarg1_, int jarg2, String jarg3);
  public final static native void delete_StringVec(long jarg1);
  public final static native long new_zmGtdModel__SWIG_0();
  public final static native long new_zmGtdModel__SWIG_1(long jarg1, MindMatterModel jarg1_);
  public final static native long zmGtdModel_const_base(long jarg1, zmGtdModel jarg1_);
  public final static native long zmGtdModel_base(long jarg1, zmGtdModel jarg1_);
  public final static native void zmGtdModel_initialize(long jarg1, zmGtdModel jarg1_);
  public final static native String zmGtdModel_getNote(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native int zmGtdModel_getImportance(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native long zmGtdModel_getInboxItems(long jarg1, zmGtdModel jarg1_, boolean jarg2);
  public final static native long zmGtdModel_getStandaloneTaskItems(long jarg1, zmGtdModel jarg1_, boolean jarg2);
  public final static native long zmGtdModel_getTaskItems(long jarg1, zmGtdModel jarg1_, boolean jarg2, boolean jarg3);
  public final static native long zmGtdModel_getProjectItems(long jarg1, zmGtdModel jarg1_, boolean jarg2, boolean jarg3);
  public final static native long zmGtdModel_getDoneItems(long jarg1, zmGtdModel jarg1_);
  public final static native long zmGtdModel_find(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native boolean zmGtdModel_isTaskItem(long jarg1, zmGtdModel jarg1_, String jarg2, boolean jarg3);
  public final static native boolean zmGtdModel_isInboxItem(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native boolean zmGtdModel_isProjectItem(long jarg1, zmGtdModel jarg1_, String jarg2, boolean jarg3);
  public final static native boolean zmGtdModel_isDone(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native String zmGtdModel_getParentProject(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native String zmGtdModel_getNextTask(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native String zmGtdModel_orderATask(long jarg1, zmGtdModel jarg1_);
  public final static native boolean zmGtdModel_itemContentMatchesString(long jarg1, zmGtdModel jarg1_, String jarg2, String jarg3);
  public final static native void zmGtdModel_print_statistics(long jarg1, zmGtdModel jarg1_);
  public final static native boolean zmGtdModel_empty(long jarg1, zmGtdModel jarg1_);
  public final static native String zmGtdModel_createNewInboxItem(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native void zmGtdModel_setNote(long jarg1, zmGtdModel jarg1_, String jarg2, String jarg3);
  public final static native void zmGtdModel_plusOne(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native void zmGtdModel_registerItemAsTask(long jarg1, zmGtdModel jarg1_, String jarg2, String jarg3);
  public final static native void zmGtdModel_setDone(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native void zmGtdModel_setDismissed(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native void zmGtdModel_castToProject(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native void zmGtdModel_castToStandaloneTask(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native void zmGtdModel_castToTaggedItem(long jarg1, zmGtdModel jarg1_, String jarg2, String jarg3);
  public final static native void zmGtdModel_setNextTask(long jarg1, zmGtdModel jarg1_, String jarg2, String jarg3);
  public final static native String zmGtdModel_createProject(long jarg1, zmGtdModel jarg1_, String jarg2);
  public final static native void delete_zmGtdModel(long jarg1);
  public final static native int MindMatterModel_Undefined_get();
  public final static native int MindMatterModel_Undirected_get();
  public final static native int MindMatterModel_Directed_get();
  public final static native long new_MindMatterModel();
  public final static native void delete_MindMatterModel(long jarg1);
  public final static native long MindMatterModel_create();
  public final static native long MindMatterModel_base(long jarg1, MindMatterModel jarg1_);
  public final static native void MindMatterModel_setConfigpersistence(long jarg1, MindMatterModel jarg1_, boolean jarg2);
  public final static native void MindMatterModel_setLocalFolder(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native String MindMatterModel_getLocalFolder(long jarg1, MindMatterModel jarg1_);
  public final static native void MindMatterModel_addDomainSyncFolder(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native void MindMatterModel_disableHashChecking(long jarg1, MindMatterModel jarg1_);
  public final static native void MindMatterModel_setTraceLevel(long jarg1, MindMatterModel jarg1_, int jarg2);
  public final static native void MindMatterModel_initialize(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_equals__SWIG_0(long jarg1, MindMatterModel jarg1_, long jarg2, MindMatterModel jarg2_, boolean jarg3);
  public final static native boolean MindMatterModel_equals__SWIG_1(long jarg1, MindMatterModel jarg1_, long jarg2, MindMatterModel jarg2_);
  public final static native boolean MindMatterModel_hasLocalChanges(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_hasRemoteChanges(long jarg1, MindMatterModel jarg1_);
  public final static native void MindMatterModel_duplicateModelTo(long jarg1, MindMatterModel jarg1_, long jarg2, MindMatterModel jarg2_);
  public final static native String MindMatterModel_createHash__SWIG_0(long jarg1, MindMatterModel jarg1_, boolean jarg2);
  public final static native String MindMatterModel_createHash__SWIG_1(long jarg1, MindMatterModel jarg1_);
  public final static native void MindMatterModel_persistence_saveLocalModel(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_persistence_loadLocalModel(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_persistence_loadSnapshot(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_persistence_createSnapshot(long jarg1, MindMatterModel jarg1_);
  public final static native void MindMatterModel_saveLocal(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_loadLocal(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_loadSnapshot(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_createSnapshot(long jarg1, MindMatterModel jarg1_);
  public final static native long MindMatterModel_diffLocal(long jarg1, MindMatterModel jarg1_);
  public final static native long MindMatterModel_diffRemote(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_sync_pull(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_sync_push(long jarg1, MindMatterModel jarg1_);
  public final static native void MindMatterModel_applyChangeSet(long jarg1, MindMatterModel jarg1_, long jarg2, ChangeSet jarg2_);
  public final static native boolean MindMatterModel_hasUsedUsername(long jarg1, MindMatterModel jarg1_);
  public final static native void MindMatterModel_setUsedUsername(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native boolean MindMatterModel_hasUsedHostname(long jarg1, MindMatterModel jarg1_);
  public final static native void MindMatterModel_setUsedHostname(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native long MindMatterModel_diffTo(long jarg1, MindMatterModel jarg1_, long jarg2, MindMatterModel jarg2_);
  public final static native void MindMatterModel_debug_dump(long jarg1, MindMatterModel jarg1_);
  public final static native boolean MindMatterModel_isConsistent(long jarg1, MindMatterModel jarg1_);
  public final static native long MindMatterModel_getItemCount(long jarg1, MindMatterModel jarg1_);
  public final static native long MindMatterModel_query(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native String MindMatterModel_completeUid(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native long MindMatterModel_getItems(long jarg1, MindMatterModel jarg1_);
  public final static native long MindMatterModel_getFolderChildren(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native long MindMatterModel_getCreationTime(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native boolean MindMatterModel_hasItem(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native String MindMatterModel_getValue(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native String MindMatterModel_getCaption(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native boolean MindMatterModel_isTag(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native boolean MindMatterModel_hasTag(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native boolean MindMatterModel_hasValue(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native boolean MindMatterModel_itemContentMatchesString(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native boolean MindMatterModel_isConnected(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native long MindMatterModel_getNeighbours(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native long MindMatterModel_getTags(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native long MindMatterModel_getLoadedJournalFiles(long jarg1, MindMatterModel jarg1_);
  public final static native String MindMatterModel_createNewItem__SWIG_0(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native String MindMatterModel_createNewItem__SWIG_1(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native void MindMatterModel_eraseItem(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native void MindMatterModel_addTag(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native void MindMatterModel_becomeFolder(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native void MindMatterModel_putIntoFolder(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native boolean MindMatterModel_removeTag(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native void MindMatterModel_setValue(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native void MindMatterModel_setCaption(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native void MindMatterModel_connectDirected(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native void MindMatterModel_disconnect(long jarg1, MindMatterModel jarg1_, String jarg2, String jarg3);
  public final static native String MindMatterModel_findOrCreateTagItem(long jarg1, MindMatterModel jarg1_, String jarg2);
  public final static native long JournalItem_createCreate(String jarg1, String jarg2);
  public final static native long JournalItem_createSetStringValue(String jarg1, String jarg2, String jarg3);
  public final static native long JournalItem_createErase(String jarg1);
  public final static native long JournalItem_createAddAttribute(String jarg1, String jarg2);
  public final static native long JournalItem_createChangeCaption(String jarg1, String jarg2);
  public final static native long JournalItem_createConnect(String jarg1, long jarg2);
  public final static native long JournalItem_createDisconnect(String jarg1, String jarg2);
  public final static native void JournalItem_item_uid_set(long jarg1, JournalItem jarg1_, String jarg2);
  public final static native String JournalItem_item_uid_get(long jarg1, JournalItem jarg1_);
  public final static native void JournalItem_type_set(long jarg1, JournalItem jarg1_, int jarg2);
  public final static native int JournalItem_type_get(long jarg1, JournalItem jarg1_);
  public final static native void JournalItem_time_set(long jarg1, JournalItem jarg1_, String jarg2);
  public final static native String JournalItem_time_get(long jarg1, JournalItem jarg1_);
  public final static native void JournalItem_key_set(long jarg1, JournalItem jarg1_, String jarg2);
  public final static native String JournalItem_key_get(long jarg1, JournalItem jarg1_);
  public final static native void JournalItem_value_set(long jarg1, JournalItem jarg1_, String jarg2);
  public final static native String JournalItem_value_get(long jarg1, JournalItem jarg1_);
  public final static native void delete_JournalItem(long jarg1);
  public final static native long new_ChangeSet__SWIG_0();
  public final static native long new_ChangeSet__SWIG_1(String jarg1);
  public final static native void delete_ChangeSet(long jarg1);
  public final static native boolean ChangeSet_write(long jarg1, ChangeSet jarg1_, String jarg2);
  public final static native long ChangeSet_getJournal(long jarg1, ChangeSet jarg1_);
  public final static native void ChangeSet_add_remove_entry(long jarg1, ChangeSet jarg1_, String jarg2);
  public final static native void ChangeSet_append(long jarg1, ChangeSet jarg1_, long jarg2);
  public final static native void ChangeSet_clear(long jarg1, ChangeSet jarg1_);
  public final static native boolean ChangeSet_isEmpty(long jarg1, ChangeSet jarg1_);
  public final static native long ChangeSet_size(long jarg1, ChangeSet jarg1_);
  public final static native void ChangeSet_debug_dump(long jarg1, ChangeSet jarg1_);
}
