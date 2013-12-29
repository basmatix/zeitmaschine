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

namespace zm
{
    class MindMatter;

    class MindMatterModel
    {
    public:

        typedef boost::bimaps::bimap< std::string, MindMatter * > uid_mm_bimap_t;

    private:

        uid_mm_bimap_t  m_things;
        std::string     m_localFolder;
        std::string     m_localModelFile;

        std::string     m_temporaryJournalFile;
        bool            m_initialized;
        bool            m_dirty;  // maybe we need more

    public:

        MindMatterModel();

        bool equals( const MindMatterModel &other, bool tell_why = false ) const;
        bool operator==( const MindMatterModel &other ) const;
        bool operator!=( const MindMatterModel &other ) const;

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

        /// make a full model sync.
        /// first load and apply new journal files in the sync folders. Then
        /// write the local model files and make the temporary journal file
        /// available to the sync folders and
        void sync();

        void applyChangeSet( const ChangeSet &changeSet );

        /// returns whether there is a valid username stored
        bool hasUsedUsername() const;

        /// returns the username used by us
        std::string getUsedUsername() const;

        /// set the username used by us - should be the system user
        /// name but doesn't have to
        void setUsedUsername( const std::string &username );

        /// returns whether there is a valid hostname stored
        bool hasUsedHostname() const;

        /// returns the hostname used by us - should be the system user
        /// name but doesn't have to
        std::string getUsedHostname() const;

        /// set the hostname used by us
        void setUsedHostname( const std::string &hostname );

        ChangeSet diff( const MindMatterModel &other ) const;

        void debug_dump() const;

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

        const uid_mm_bimap_t & things() const;

        size_t getItemCount() const;

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

private:
        bool _isConnected(
                uid_mm_bimap_t::left_const_iterator item_it1,
                uid_mm_bimap_t::left_const_iterator item_it2 ) const;

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

        void connect(
                const std::string &node1_uid,
                const std::string &node2_uid );

        void disconnect(
                const std::string &node1_uid,
                const std::string &node2_uid );

        std::string findOrCreateTagItem(
                const std::string &name );

    private:

        void _createNewItem(
                const std::string &uid,
                const std::string &caption,
                const std::string &time );

        void _eraseItem(
                uid_mm_bimap_t::left_iterator &item );

        void _addTag(
                uid_mm_bimap_t::left_iterator &item,
                const std::string &tag_name );

        bool _removeTag(
                uid_mm_bimap_t::left_iterator &item,
                const std::string &tag_name );

        bool _setValue(
                uid_mm_bimap_t::left_iterator &item,
                const std::string &name,
                const std::string &value );

        bool _setCaption(
                uid_mm_bimap_t::left_iterator &item,
                const std::string &caption );

        void _connect(
                uid_mm_bimap_t::left_iterator &item1,
                uid_mm_bimap_t::left_iterator &item2 );

        void _disconnect(
                uid_mm_bimap_t::left_iterator &item1,
                uid_mm_bimap_t::left_iterator &item2 );

        // returns 16x8 bit
        static std::string generateUid();

        static void clear( uid_mm_bimap_t &thingsMap );
    };
}

#endif
