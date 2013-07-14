/// -*- coding: utf-8 -*-
///
/// file: zmModel.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef FLOWMODEL_H
#define FLOWMODEL_H

#include "zmChangeSet.h"

#include <boost/bimap.hpp>
#include <string>
#include <list>

namespace zm
{
    class MindMatter;

    class MindMatterModel
    {
    public:

        typedef boost::bimaps::bimap< std::string, MindMatter * > MindMatterModelMapType;

    private:

        MindMatterModelMapType  m_things;
        std::string         m_localFolder;
        std::string         m_localModelFile;
        ChangeSet           m_changeSet;

        std::string         m_temporaryJournalFile;
        bool                m_initialized;
        bool                m_dirty;  // maybe we need more

    public:

        MindMatterModel();

        bool operator==( const MindMatterModel &other );
        bool operator!=( const MindMatterModel &other );

        void setConfigPersistance( bool value );

        /// sets the one and only local folder for configuration temorary and
        /// snapshot files
        void setLocalFolder( const std::string &path );

        /// defines a (externally synced) folder exchange with other clients
        /// and/or users
        void addDomainSyncFolder( const std::string &domainName, const std::string &path );

        /// will initialize the model by loading the persistant state using
        /// the given information
        void initialize();

        /// will write recent changes to the journal file
        void localSave();

        /// make a fulf model sync.
        /// first load and apply new journal files in the sync folders. Then
        /// write the local model files and make the temporary journal file
        /// available to the sync folders and
        void sync();

        /// development only: load an external model file and merge items
        /// which don't exist in the current model
        void merge( const std::string &modelFile );

        /// returns whether there is a valid username stored
        bool hasUsedUsername() const;

        /// returns whether there is a valid hostname stored
        bool hasUsedHostname() const;

        /// returns the username used by us
        std::string getUsedUsername() const;

        /// set the username used by us - should be the system user
        /// name but doesn't have to
        void setUsedUsername( const std::string &username );

        /// returns the hostname used by us - should be the system user
        /// name but doesn't have to
        std::string getUsedHostname() const;

        /// set the hostname used by us
        void setUsedHostname( const std::string &hostname );

        void applyChangeSet( const ChangeSet &changeSet );

        ChangeSet diff( const MindMatterModel &other );

    private:

        void loadLocalModel( const std::string &filename );

        bool importJournalFiles();

        void dirty();

        void saveLocalModel( const std::string &filename );

        void makeTempJournalStatic();

        /// returns a sorted list containing names of journal files
        /// located in the sync folder
        std::vector< std::string > getJournalFiles();

    /// const interface
    public:

        const MindMatterModelMapType & things() const;

        bool equals( const MindMatterModelMapType &thingsMap, const MindMatterModelMapType &thingsMapOther ) const;

        size_t getItemCount() const;

        std::time_t getCreationTime( const std::string &uid ) const;

        bool hasItem( const std::string &uid ) const;

        std::string getValue( const std::string &uid, const std::string &name ) const;

        const std::string & getCaption( const std::string &uid ) const;

        bool hasTag( const std::string &uid, const std::string &tag_name ) const;

        bool hasValue( const std::string &uid, const std::string &name ) const;

        bool itemContentMatchesString( const std::string &uid, const std::string &searchString ) const;

        bool isConnected( const std::string &node1_uid, const std::string &node2_uid ) const;

        std::list< std::string > getNeighbours( const std::string &node_uid );

private:
        bool _isConnected(
                MindMatterModelMapType::left_const_iterator item_it1,
                MindMatterModelMapType::left_const_iterator item_it2 ) const;

    /// write relevant interface
    public:

        std::string createNewItem( const std::string &caption );

        void eraseItem( const std::string &uid );

        void addTag( const std::string &uid, const std::string &tag_name );

        bool removeTag( const std::string &uid, const std::string &tag_name );

        void setValue( const std::string &uid, const std::string &name, const std::string &value );

        void setCaption( const std::string &uid, const std::string &caption );

        void connect( const std::string &node1_uid, const std::string &node2_uid );

        void disconnect( const std::string &node1_uid, const std::string &node2_uid );

        std::string findOrCreateTagItem( const std::string &name );

    private:

        void _createNewItem( const std::string &uid, const std::string &caption, const std::string &time );

        void _eraseItem( MindMatterModelMapType::left_iterator &item );

        void _addTag( MindMatterModelMapType::left_iterator &item, const std::string &tag_name );

        bool _removeTag( MindMatterModelMapType::left_iterator &item, const std::string &tag_name );

        bool _setValue( MindMatterModelMapType::left_iterator &item, const std::string &name, const std::string &value );

        bool _setCaption( MindMatterModelMapType::left_iterator &item, const std::string &caption );

        void _connect( MindMatterModelMapType::left_iterator &item1, MindMatterModelMapType::left_iterator &item2 );

        // returns 16x8 bit
        static std::string generateUid();

        static void clear( MindMatterModelMapType &thingsMap );
    };
}

#endif
