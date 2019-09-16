<?php

namespace AppBundle\Admin\Customer;

use AppBundle\Entity\Customer\E_Customer;
use Sonata\AdminBundle\Admin\AbstractAdmin;
use Sonata\AdminBundle\Datagrid\DatagridMapper;
use Sonata\AdminBundle\Datagrid\ListMapper;
use Sonata\AdminBundle\Form\FormMapper;
use Sonata\AdminBundle\Show\ShowMapper;
use Sonata\AdminBundle\Route\RouteCollection;

use AppBundle\Entity\Customer\E_Company;

class Admin__E_Customer extends AbstractAdmin
{
    protected $classnameLabel = 'Kunde';

    protected function configureRoutes(RouteCollection $collection)
    {
    }

    /**
     * @param DatagridMapper $datagridMapper
     */
    protected function configureDatagridFilters(DatagridMapper $datagridMapper)
    {
        $datagridMapper
            ->add('id')
            ->add('enabled', null, array( 'label' => 'Aktiviert'))
            ->add('title', null, array( 'label' => 'Titel'))
            ->add('name__first', null, array( 'label' => 'Vorname'))
            ->add('name__last', null, array( 'label' => 'Nachname'))
            ->add('birthdate', null, array( 'label' => 'Geburtstag'))
            ->add('address__street', null, array( 'label' => 'Anschrift: Straße'))
            ->add('address__addition', null, array( 'label' => 'Anschrift: Zusatz'))
            ->add('address__postalcode', null, array( 'label' => 'Anschrift: PLZ'))
            ->add('address__location', null, array( 'label' => 'Anschrift: Ort'))
            ->add('contact__tel_main', null, array( 'label' => 'Festnetz'))
            ->add('contact__tel_mobile', null, array( 'label' => 'Mobil'))
            ->add('contact__tel_fax', null, array( 'label' => 'Fax'))
            ->add('contact__email', null, array( 'label' => 'E-Mail'))
            ->add('newsletter', null, array( 'label' => 'Newsletter'))
            ->add('payment__bank__account_owner', null, array( 'label' => 'Kontoinhaber'))
            ->add('payment__bank__account_number', null, array( 'label' => 'Kontonummer'))
            ->add('payment__bank__bic', null, array( 'label' => 'BIC'))
            ->add('payment__bank__iban', null, array( 'label' => 'IBAN'))
            ->add('fk_e_company__id', null, array( 'label' => 'Firma'))
            ->add('fk_fos_user__id', null, array( 'label' => 'Benutzeraccount'))
        ;
    }

    /**
     * @param ListMapper $listMapper
     */
    protected function configureListFields(ListMapper $listMapper)
    {
        $listMapper
            ->add('id', null, array('header_style' => 'width: 75px;'))
            ->add('enabled', null, array( 'label' => 'Aktiv'))
            ->add('name__first', null, array( 'label' => 'Vorname'))
            ->add('name__last', null, array( 'label' => 'Nachname'))
            ->add('newsletter')
            ->add('_action', null, array(
                'actions' => array(
                    'show' => array(),
                    'edit' => array(),
                    'delete' => array(),
                )
            ))
        ;
    }

    /**
     * @param FormMapper $formMapper
     */
    protected function configureFormFields(FormMapper $formMapper)
    {
        $formMapper
            ->with('Kunde', array('class'       => 'col-md-4' ))
                ->add('enabled', null, array('label' => 'Aktiviert', 'required' => true))
                ->add('newsletter', null, array( 'label' => 'Newsletter'))
                ->add('fk_fos_user__id', 'entity', array(
                    'label'     => 'User Account',
                    'class'     => 'Application\Sonata\UserBundle\Entity\User',
                    'property'  => 'username',
                    'empty_value'   => '- nicht registriert -',
                    'required'      => false,
                    'label'         => 'Benutzeraccount'
                ))
                ->add('fk_e_company__id', 'entity', array(
                    'label'         => 'Company',
                    'class'        => 'AppBundle\Entity\Customer\E_Company',
                    'property'      => 'name',
                    'empty_value'   => '- Privatkunde -',
                    'required'      => false,
                    'label'         => 'Firma'
                ))
            ->end()

            ->with('Person', array('class'       => 'col-md-4' ))
                ->add('title', null, array( 'label' => 'Titel'))
                ->add('name__first', null, array( 'label' => 'Vorname'))
                ->add('name__last', null, array( 'label' => 'Nachname'))
                ->add('birthdate', 'sonata_type_date_picker', array( 'label' => 'Geburtstag', 'required'=>false))
            ->end()

            ->with('Kontakt', array('class'       => 'col-md-4' ))
            ->add('contact__tel_main', null, array( 'label' => 'Festnetz'))
            ->add('contact__tel_mobile', null, array( 'label' => 'Mobil'))
            ->add('contact__tel_fax', null, array( 'label' => 'Fax'))
            ->add('contact__email', null, array( 'label' => 'E-Mail'))
            ->end()

            ->with('Anschrift', array('class'       => 'col-md-6' ))
                ->add('address__street', null, array( 'label' => 'Anschrift: Straße'))
                ->add('address__addition', null, array( 'label' => 'Anschrift: Zusatz'))
                ->add('address__postalcode', null, array( 'label' => 'Anschrift: PLZ'))
                ->add('address__location', null, array( 'label' => 'Anschrift: Ort'))
            ->end()

            ->with('Bankdaten', array('class'       => 'col-md-6' ))
                ->add('payment__bank__account_owner', null, array( 'label' => 'Kontoinhaber'))
                ->add('payment__bank__account_number', null, array( 'label' => 'Kontonummer'))
                ->add('payment__bank__bic', null, array( 'label' => 'BIC'))
                ->add('payment__bank__iban', null, array( 'label' => 'IBAN'))
            ->end()
        ;
    }

    /**
     * @param ShowMapper $showMapper
     */
    protected function configureShowFields(ShowMapper $showMapper)
    {
        $showMapper
            ->add('id')
            ->add('enabled', null, array('label' => 'Aktiviert'))
            ->add('title', null, array( 'label' => 'Titel'))
            ->add('name__first', null, array( 'label' => 'Vorname'))
            ->add('name__last', null, array( 'label' => 'Nachname'))
            ->add('birthdate', null, array( 'label' => 'Geburtstag'))
            ->add('address__street', null, array( 'label' => 'Anschrift: Straße'))
            ->add('address__addition', null, array( 'label' => 'Anschrift: Zusatz'))
            ->add('address__postalcode', null, array( 'label' => 'Anschrift: PLZ'))
            ->add('address__location', null, array( 'label' => 'Anschrift: Ort'))
            ->add('contact__tel_main', null, array( 'label' => 'Festnetz'))
            ->add('contact__tel_mobile', null, array( 'label' => 'Mobil'))
            ->add('contact__tel_fax', null, array( 'label' => 'Fax'))
            ->add('contact__email', null, array( 'label' => 'E-Mail'))
            ->add('newsletter', null, array( 'label' => 'Newsletter'))
            ->add('payment__bank__account_owner', null, array( 'label' => 'Kontoinhaber'))
            ->add('payment__bank__account_number', null, array( 'label' => 'Kontonummer'))
            ->add('payment__bank__bic', null, array( 'label' => 'BIC'))
            ->add('payment__bank__iban', null, array( 'label' => 'IBAN'))
            ->add('fk_e_company__id', null, array( 'label' => 'Firma'))
            ->add('fk_fos_user__id', null, array( 'label' => 'Benutzeraccount'))
        ;
    }

    public function toString($object)
    {
        return $object instanceof E_Customer
            ? $object->getName()
            : $this->classnameLabel;
    }
}
