/// -*- coding: utf-8 -*-
///
/// file: zmModel.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef FLOWMODEL_H
#define FLOWMODEL_H

// MindMatter creates ChangeSet instances
#include "zmChangeSet.h"
#include "zmModelData.h"

#include <boost/bimap.hpp>
#include <string>
#include <list>

namespace YAML
{
    class Node;
}

namespace zm
{
    class MindMatter;
    class ChangeSet;
    class zmOptions;


    /// rebuilds a string <=> MindMatter bimap interface and provides
    /// Map related functionality like serialization
    class MindMatterModel
    {
    public:


        enum ConnectionType
        {
            Undefined = 0,
            Undirected = 1,
            Directed = 2
        };

    private:

        ModelData       m_things;

        // used for generating the diff
        ModelData       m_things_synced;

        zmOptions      *m_options;


        std::string     m_localFolderRoot;
        std::string     m_localFolderSync;
        std::string     m_localModelFile;
        std::string     m_localModelFileSynced;

        bool            m_initialized;
        bool            m_checkHashes;

        MindMatterModel(const MindMatterModel &);
        MindMatterModel & operator=(const MindMatterModel &);

        // forbidden because some languages don't support it (we want to SWIG)
        bool operator==( const MindMatterModel &other ) const;
        bool operator!=( const MindMatterModel &other ) const;

    public:

        MindMatterModel();
        ~MindMatterModel();

        static boost::shared_ptr< MindMatterModel > create();

        boost::shared_ptr< MindMatterModel > base();

        void setConfigpersistence(
                bool value );

        /// sets the one and only local folder for configuration temorary and
        /// snapshot files
        void setLocalFolder(
                const std::string &path );

        const std::string & getLocalFolder() const;

        /// defines a (externally synced) folder exchange with other clients
        /// and/or users
        void addDomainSyncFolder(
                const std::string &domainName,
                const std::string &path );

        void disableHashChecking();

        void setTraceLevel(
                int level);

        /// will initialize the model by loading the persistant state using
        /// the given information
        void initialize();

        /// compares the model with another one and returns true if equal
        bool equals(
                const MindMatterModel &other, bool tell_why = false ) const;

        void duplicateModelTo(
                      MindMatterModel &other) const;

        std::string createHash(
                bool verbose=false ) const;

        /// will write a recent model file without affecting the journals
        void persistence_saveLocalModel();

        /// loads a serialized representation from fs
        bool persistence_loadLocalModel();

        /// load a recent snapshot
        bool persistence_loadSnapshot();

        /// load a new snapshot from current model
        bool persistence_createSnapshot();

        /// make a full model sync.
        /// first load and apply new journal files in the sync folders. Then
        /// write the local model files and make the temporary journal file
        /// available to the sync folders and
        bool persistence_sync();

        /// for convenience: see persistence_saveLocalModel()
        void saveLocal();

        /// for convenience: see persistence_loadLocalModel()
        bool loadLocal();

        /// for convenience: see persistence_loadSnapshot()
        bool loadSnapshot();

        /// for convenience: see persistence_createSnapshot()
        bool createSnapshot();

        /// for convenience: see persistence_sync()
        bool sync();

        void applyChangeSet(
                const ChangeSet &changeSet );

        /// returns whether there is a valid username stored
        bool hasUsedUsername() const;

        /// returns the username used by us
        //std::string getUsedUsername() const;

        /// set the username used by us - should be the system user
        /// name but doesn't have to
        void setUsedUsername(
                const std::string &username );

        /// returns whether there is a valid hostname stored
        bool hasUsedHostname() const;

        /// returns the hostname used by us - should be the system user
        /// name but doesn't have to
        //std::string getUsedHostname() const;

        /// set the hostname used by us
        void setUsedHostname(
                const std::string &hostname );

        ChangeSet diffTo(
                const MindMatterModel &other ) const;

        void debug_dump() const;

    private:

        std::string createJournalFileName() const;
        std::string createModelFileNameNew() const;
        std::string createModelFileNameOld() const;
        std::string createJournalListFileName() const;
        std::string createSnapshotFileName() const;

        static void yamlToThingsMap(
                const YAML::Node  &yamlNode,
                      ModelData   &thingsMap,
                      bool         checkHashes);

        // todo - refactor name
        static void _saveModel(
                const ModelData   &model,
                const std::string &filename);

        static void deepCopy(
                const ModelData   &source,
                      ModelData   &dest);

        static ChangeSet diff(
                const ModelData   &model_from,
                const ModelData   &model_to);

        static bool equals(
                const ModelData   &a_first,
                const ModelData   &a_second,
                      bool         tell_why);

        ChangeSet persistence_pullJournal();

        ChangeSet persistence_pushJournal();

        /// returns a sorted list containing names of journal files
        /// located in the sync folder
        std::vector< std::string > getJournalFiles() const;

    /// const interface
    public:

        size_t getItemCount() const;

        uid_lst_t query(
                const std::string &query_str) const;

        uid_lst_t getItems() const;

        uid_lst_t getFolderChildren( 
                const uid_t       &folder_item ) const;

        std::time_t getCreationTime(
                const zm::uid_t   &uid ) const;

        bool hasItem(
                const zm::uid_t   &uid ) const;

        std::string getValue(
                const zm::uid_t   &uid,
                const std::string &name ) const;

        const std::string & getCaption(
                const zm::uid_t   &uid ) const;

        bool hasTag(
                const zm::uid_t   &uid,
                const std::string &tag_name ) const;

        bool hasValue(
                const zm::uid_t   &uid,
                const std::string &name ) const;

        bool itemContentMatchesString(
                const zm::uid_t   &uid,
                const std::string &searchString ) const;

        bool isConnected(
                const zm::uid_t   &node1_uid,
                const zm::uid_t   &node2_uid ) const;

        uid_lst_t getNeighbours(
                const zm::uid_t   &node_uid ) const;

    private: // const
        bool _isConnected(
                ModelData::left_const_iterator item_it1,
                ModelData::left_const_iterator item_it2 ) const;

    /// write relevant interface
    public:

        zm::uid_t createNewItem(
                const std::string &caption,
                const zm::uid_t   &uid = "" );

        void eraseItem(
                const zm::uid_t   &uid );

        void addTag(
                const zm::uid_t   &uid,
                const std::string &tag_name );

        void becomeFolder(
                const uid_t       &item );

        void putIntoFolder(
                const zm::uid_t   &item,
                const zm::uid_t   &folder);

        bool removeTag(
                const zm::uid_t   &uid,
                const std::string &tag_name );

        void setValue(
                const zm::uid_t   &uid,
                const std::string &name,
                const std::string &value );

        void setCaption(
                const zm::uid_t   &uid,
                const std::string &caption );

        void connectDirected(
                const zm::uid_t   &node1_uid,
                const zm::uid_t   &node2_uid );

        void disconnect(
                const zm::uid_t   &node1_uid,
                const zm::uid_t   &node2_uid );

        zm::uid_t findOrCreateTagItem(
                const std::string &name );

    private: // rw

        static zm::uid_t _createNewItem(
                      ModelData   &model,
                const std::string &caption,
                const zm::uid_t   &uid = "" );

        static void _createNewItem(
                      ModelData   &model,
                const zm::uid_t   &uid,
                const std::string &caption,
                const std::string &time );

        void _eraseItem(
                ModelData::left_iterator &item );

        static void _addTag(
                ModelData         &model,
                ModelData::left_iterator &item,
                const std::string &tag_name );

        bool _removeTag(
                ModelData::left_iterator &item,
                const std::string &tag_name );

        static bool _setValue(
                ModelData::left_iterator &item,
                const std::string &name,
                const std::string &value );

        static bool _setCaption(
                ModelData::left_iterator &item,
                const std::string &caption );

        static void _connectDuplex(
                ModelData::left_iterator &item1,
                ModelData::left_iterator &item2,
                ConnectionType     type);

        static void _connectSingle(
                ModelData::left_iterator &item1,
                ModelData::left_iterator &item2,
                int                type);

        static void _disconnect(
                ModelData::left_iterator &item1,
                ModelData::left_iterator &item2 );

        // returns 16x8 bit
        static zm::uid_t generateUid();

        static void clear(
                      ModelData     &thingsMap );

        static bool loadModelFromFile(
                const std::string   &input_file,
                      ModelData     &rw_thingsMap,
                      bool           checkHashes);
    };
}

#endif
