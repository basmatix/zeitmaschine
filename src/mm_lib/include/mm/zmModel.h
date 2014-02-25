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

        class ModelData
        {
            typedef boost::bimaps::bimap< std::string, MindMatter * > uid_mm_bimap_t;
            uid_mm_bimap_t m_data;

        public:

            std::set< std::string > m_read_journals;

            ModelData()
                : m_data    ()
                , right     (m_data.right)
                , left      (m_data.left)
                , m_read_journals()
            { }

            typedef uid_mm_bimap_t::left_const_iterator left_const_iterator;
            typedef uid_mm_bimap_t::left_iterator left_iterator;
            typedef uid_mm_bimap_t::right_const_iterator right_const_iterator;
            typedef uid_mm_bimap_t::right_iterator right_iterator;
            typedef uid_mm_bimap_t::const_iterator const_iterator;
            typedef uid_mm_bimap_t::iterator iterator;

            typedef uid_mm_bimap_t::value_type value_type;

            virtual ~ModelData(){}

            size_t size() const {return m_data.size();}
            bool empty() const {return m_data.empty();}
            void clear() {m_data.clear();}
            void insert(value_type a_pair) {m_data.insert(a_pair);}
            uid_mm_bimap_t::iterator begin() {return m_data.begin();}
            uid_mm_bimap_t::iterator end() {return m_data.end();}
            uid_mm_bimap_t::const_iterator begin() const {return m_data.begin();}
            uid_mm_bimap_t::const_iterator end() const{return m_data.end();}

            uid_mm_bimap_t::left_map  &left;
            uid_mm_bimap_t::right_map &right;

        public:
            void debug_dump() const;

        };

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

    public:

        MindMatterModel();
        ~MindMatterModel();

        static boost::shared_ptr<MindMatterModel> create()
        {
            return boost::shared_ptr<MindMatterModel>(
                        new MindMatterModel() );
        }

        void setConfigPersistance( bool value );

        /// sets the one and only local folder for configuration temorary and
        /// snapshot files
        void setLocalFolder( const std::string &path );

        const std::string & getLocalFolder() const;

        /// defines a (externally synced) folder exchange with other clients
        /// and/or users
        void addDomainSyncFolder(
                const std::string &domainName,
                const std::string &path );

        void disableHashChecking();

        void setTraceLevel(int level);

        /// will initialize the model by loading the persistant state using
        /// the given information
        void initialize();

        bool equals( const MindMatterModel &other, bool tell_why = false ) const;
        bool operator==( const MindMatterModel &other ) const;
        bool operator!=( const MindMatterModel &other ) const;

        void duplicateModelTo(MindMatterModel &other) const;

        std::string createHash( bool verbose=false ) const;

        /// will write a recent model file without affecting the journals
        void persistence_saveLocalModel();

        /// loads
        bool persistence_loadLocalModel();

        bool persistance_loadSnapshot();
        bool persistance_createSnapshot();

        /// make a full model sync.
        /// first load and apply new journal files in the sync folders. Then
        /// write the local model files and make the temporary journal file
        /// available to the sync folders and
        bool persistence_sync();

        void applyChangeSet( const ChangeSet &changeSet );

        /// returns whether there is a valid username stored
        bool hasUsedUsername() const;

        /// returns the username used by us
        //std::string getUsedUsername() const;

        /// set the username used by us - should be the system user
        /// name but doesn't have to
        void setUsedUsername( const std::string &username );

        /// returns whether there is a valid hostname stored
        bool hasUsedHostname() const;

        /// returns the hostname used by us - should be the system user
        /// name but doesn't have to
        //std::string getUsedHostname() const;

        /// set the hostname used by us
        void setUsedHostname( const std::string &hostname );

        ChangeSet diffTo( const MindMatterModel &other ) const;

        void debug_dump() const;

    private:

        std::string createJournalFileName() const;
        std::string createModelFileNameNew() const;
        std::string createModelFileNameOld() const;
        std::string createJournalListFileName() const;
        std::string createSnapshotFileName() const;

//        const std::set< std::string > & getHandledJournalFilenames();
//        void appendHandledJournalFilename(const std::string &filename);

        static void yamlToThingsMap(
                const YAML::Node  &yamlNode,
                ModelData         &thingsMap,
                bool               checkHashes);

        // todo - refactor name
        static void _saveModel(
                const ModelData   &model,
                const std::string &filename);

        static void deepCopy(
                const ModelData &source,
                      ModelData &dest);

        static ChangeSet diff(
                const ModelData &model_from,
                const ModelData &model_to);

        static bool equals(
                const ModelData &a_first,
                const ModelData &a_second,
                bool tell_why);

        ChangeSet persistence_pullJournal();

        ChangeSet persistence_pushJournal();

        /// returns a sorted list containing names of journal files
        /// located in the sync folder
        std::vector< std::string > getJournalFiles() const;

    /// const interface
    public:

        // should be deprecated
        const ModelData & things() const;

        size_t getItemCount() const;

        std::vector< std::string > getItems() const;

        std::time_t getCreationTime(
                const std::string &uid ) const;

        bool hasItem(
                const std::string &uid ) const;

        std::string getValue(
                const std::string &uid,
                const std::string &name ) const;

        const std::string & getCaption(
                const std::string &uid ) const;

        bool hasTag(
                const std::string &uid,
                const std::string &tag_name ) const;

        bool hasValue(
                const std::string &uid,
                const std::string &name ) const;

        bool itemContentMatchesString(
                const std::string &uid,
                const std::string &searchString ) const;

        bool isConnected(
                const std::string &node1_uid,
                const std::string &node2_uid ) const;

        std::list< std::string > getNeighbours(
                const std::string &node_uid ) const;

    private: // const
        bool _isConnected(
                ModelData::left_const_iterator item_it1,
                ModelData::left_const_iterator item_it2 ) const;

    /// write relevant interface
    public:

        std::string createNewItem(
                const std::string &caption,
                const std::string &uid = "" );

        void eraseItem(
                const std::string &uid );

        void addTag(
                const std::string &uid,
                const std::string &tag_name );

        bool removeTag(
                const std::string &uid,
                const std::string &tag_name );

        void setValue(
                const std::string &uid,
                const std::string &name,
                const std::string &value );

        void setCaption(
                const std::string &uid,
                const std::string &caption );

        void connectDirected(
                const std::string &node1_uid,
                const std::string &node2_uid );

        void disconnect(
                const std::string &node1_uid,
                const std::string &node2_uid );

        std::string findOrCreateTagItem(
                const std::string &name );

    private: // rw

        static std::string _createNewItem(
                ModelData    &model,
                const std::string &caption,
                const std::string &uid = "" );

        static void _createNewItem(
                ModelData    &model,
                const std::string &uid,
                const std::string &caption,
                const std::string &time );

        void _eraseItem(
                ModelData::left_iterator &item );

        static void _addTag(
                ModelData                &model,
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
                ConnectionType type);

        static void _connectSingle(
                ModelData::left_iterator &item1,
                ModelData::left_iterator &item2,
                int type);

        static void _disconnect(
                ModelData::left_iterator &item1,
                ModelData::left_iterator &item2 );

        // returns 16x8 bit
        static std::string generateUid();

        static void clear( ModelData &thingsMap );

        static bool loadModelFromFile(
                const std::string   &input_file,
                ModelData           &rw_thingsMap,
                bool                 checkHashes);
    };
}

#endif
